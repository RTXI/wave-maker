/*
			Copyright (C) 2011 Georgia Institute of Technology

			This program is free software: you can redistribute it and/or modify
			it under the terms of the GNU General Public License as published by
			the Free Software Foundation, either version 3 of the License, or
			(at your option) any later version.

			This program is distributed in the hope that it will be useful,
			but WITHOUT ANY WARRANTY; without even the implied warranty of
			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
			GNU General Public License for more details.

			You should have received a copy of the GNU General Public License
			along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <wave-maker.h>
#include <math.h>
#include <time.h>
#include <main_window.h>

extern "C" Plugin::Object *createRTXIPlugin(void)
{
	return new WaveMaker(); // Change the name of the plug-in here
}

static DefaultGUIModel::variable_t vars[] = {
	{ "Output", "Signal from File", DefaultGUIModel::OUTPUT, },
	{
		"Loops", "Number of Times to Loop Data From File",
		DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
	},
	{
		"Length (s)", "Length of Trial is Computed From the Real-Time Period",
		DefaultGUIModel::STATE,
	},
	{
		"Gain", "Factor to Amplify the Signal", DefaultGUIModel::PARAMETER
			| DefaultGUIModel::DOUBLE,
	},
	{ "File Name", "ASCII Input File", DefaultGUIModel::COMMENT, },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

WaveMaker::WaveMaker(void) : DefaultGUIModel("Wave Maker", ::vars, ::num_vars)
{
	setWhatsThis(
			"<p><b>Wave Maker:</b><br>This module reads a single value from an ascii file and outputs it as a signal.</p>");

	initParameters();
	DefaultGUIModel::createGUI(vars, num_vars);
	customizeGUI();
	update(INIT);
	refresh();
	QTimer::singleShot(0, this, SLOT(resizeMe()));
}

WaveMaker::~WaveMaker(void) {}

void WaveMaker::execute(void)
{
	if ((nloops && loop >= nloops) || !wave.size()) {
		pauseButton->setChecked(true);
		return;
	}

	output(0) = wave[idx++] * gain;

	if (idx >= wave.size()) {
		idx = 0;
		if (nloops) ++loop;
	}
}

void WaveMaker::customizeGUI(void)
{
	QGridLayout *customlayout = DefaultGUIModel::getLayout();

	QGroupBox *fileBox = new QGroupBox("File");
	QHBoxLayout *fileBoxLayout = new QHBoxLayout;
	fileBox->setLayout(fileBoxLayout);
	QPushButton *loadBttn = new QPushButton("Load File");
	fileBoxLayout->addWidget(loadBttn);
	QPushButton *previewBttn = new QPushButton("Preview File");
	fileBoxLayout->addWidget(previewBttn);
	QObject::connect(loadBttn, SIGNAL(clicked()), this, SLOT(loadFile()));
	QObject::connect(previewBttn, SIGNAL(clicked()), this, SLOT(previewFile()));

	customlayout->addWidget(fileBox, 0, 0);
	setLayout(customlayout);
}

void WaveMaker::update(DefaultGUIModel::update_flags_t flag)
{
	switch (flag) {
		case INIT:
			setParameter("Loops", QString::number(nloops));
			setParameter("Gain", QString::number(gain));
			setComment("File Name", filename);
			setState("Length (s)", length);
			break;

		case MODIFY:
			nloops = getParameter("Loops").toUInt();
			gain = getParameter("Gain").toDouble();
			filename = getComment("File Name");
			break;

		case PAUSE:
			output(0) = 0; // stop command in case pause occurs in the middle of command
			idx = 0;
			loop = 0;
			gain = 0;
			break;

		case UNPAUSE:
			break;

		case PERIOD:
			dt = RT::System::getInstance()->getPeriod() * 1e-9;
			loadFile(filename);

		default:
			break;
	}
}

void WaveMaker::initParameters()
{
	dt = RT::System::getInstance()->getPeriod() * 1e-9; // s
	gain = 1;
	filename = "No file loaded.";
	idx = 0;
	loop = 0;
	nloops = 1;
	length = 0;
}

void WaveMaker::loadFile()
{
	QFileDialog* fd = new QFileDialog(this, "Wave Maker Input File");//, TRUE);
	fd->setFileMode(QFileDialog::AnyFile);
	fd->setViewMode(QFileDialog::Detail);
	QString fileName;
	if (fd->exec() == QDialog::Accepted) {
		QStringList files = fd->selectedFiles();
		if (!files.isEmpty()) fileName = files.takeFirst();
		setComment("File Name", fileName);
		wave.clear();
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly)) {
			QTextStream stream(&file);
			double value;
			while (!stream.atEnd()) {
				stream >> value;
				wave.push_back(value);
			}
			filename = fileName;
		}
		length = wave.size() * dt;
		setState("Length (s)", length); // initialized in s, display in s
	} else setComment("File Name", "No file loaded.");
}

void WaveMaker::loadFile(QString fileName)
{
	if (fileName == "No file loaded.") {
		return;
	} else {
		wave.clear();
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly)) {
			QTextStream stream(&file);
			double value;
			while (!stream.atEnd()) {
				stream >> value;
				wave.push_back(value);
			}
		}
		length = wave.size() * dt;
		setState("Length (s)", length); // initialized in s, display in s
	}
}

void WaveMaker::previewFile()
{
	double* time = new double[static_cast<int> (wave.size())];
	double* yData = new double[static_cast<int> (wave.size())];
	for (int i = 0; i < wave.size(); i++) {
		time[i] = dt * i;
		yData[i] = wave[i];
	}
	PlotDialog *preview = new PlotDialog(this, "Wave Maker Waveform", time, yData, wave.size());

	preview->show();
}
