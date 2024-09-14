/*
  Copyright (C) 2011 Georgia Institute of Technology

  This program is free software: you can redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

  This program is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
   the GNU General Public License for more details.

  You should have received a copy of the GNU General
   Public License along with this program.  If not, see
   <http://www.gnu.org/licenses/>.

*/

#include <QBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <cmath>

#include "widget.hpp"

#include <rtxi/rtos.hpp>

wave_maker::Plugin::Plugin(Event::Manager* ev_manager)
    : Widgets::Plugin(ev_manager, std::string(wave_maker::MODULE_NAME))
{
}

wave_maker::Panel::Panel(QMainWindow* main_window, Event::Manager* ev_manager)
    : Widgets::Panel(
          std::string(wave_maker::MODULE_NAME), main_window, ev_manager)
    , filenameLineEdit(new QLineEdit)
{
  setWhatsThis(
      "This module loads data from an ASCII formatted file. It samples one "
      "value from the the file on every time step and creates and generates an "
      "output signal. The module computes the time length of the waveform "
      "based on the current real-time period. This is an easy way to test your "
      "algorithms on pre-recorded data as if it were being acquired in "
      "real-time.");

  createGUI(get_default_vars(), {});
  customizeGUI();
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

wave_maker::Component::Component(Widgets::Plugin* hplugin)
    : Widgets::Component(hplugin,
                         std::string(wave_maker::MODULE_NAME),
                         wave_maker::get_default_channels(),
                         wave_maker::get_default_vars())
{
}

void wave_maker::Component::updateParameters()
{
  nloops = getValue<uint64_t>(PARAMETER::LOOP);
  gain = getValue<double>(PARAMETER::GAIN);
  length =
      (static_cast<int64_t>(wave.size()) * dt) / RT::OS::SECONDS_TO_NANOSECONDS;
  // Let UI side know how long the trial is
  setValue<int64_t>(PARAMETER::LENGTH, length);
  idx = 0;
}

void wave_maker::Component::execute()
{
  // This is the real-time function that will be called
  switch (this->getState()) {
    case RT::State::EXEC:
      if (loop >= nloops) {
        setState(RT::State::PAUSE);
        return;
      }
      writeoutput(0, wave[idx++] * gain);
      idx = idx % wave.size();
      loop = idx / wave.size();
      break;
    case RT::State::INIT:
      loadWave();
      updateParameters();
      wave.empty() ? setState(RT::State::PAUSE) : setState(RT::State::EXEC);
      break;
    case RT::State::MODIFY:
      updateParameters();
      loadWave();
      setState(RT::State::PAUSE);
      break;
    case RT::State::PAUSE:
      writeoutput(0, 0);
      break;
    case RT::State::UNPAUSE:
      // We can only run the plugin if there is data
      wave.empty() ? setState(RT::State::PAUSE) : setState(RT::State::EXEC);
      break;
    case RT::State::PERIOD:
      dt = RT::OS::getPeriod();
      // In the case of a period change while this plugin is
      // running, we need to let the UI side update wave
      // parameters i.e. create the vector before we can
      // load the wave data.
      setState(RT::State::PAUSE);
      break;
    default:
      break;
  }
}

void wave_maker::Component::initParameters()
{
  dt = RT::OS::getPeriod();  // ns
  gain = 1;
  setValue<double>(PARAMETER::GAIN, 1.0);
  idx = 0;
  loop = 1;
  setValue<uint64_t>(PARAMETER::LOOP, 1);
  nloops = 1;
  loadWave();
  if (!wave.empty()) {
    length = dt / static_cast<int64_t>(wave.size());
  } else {
    length = 0;
  }
  setValue<int64_t>(PARAMETER::LENGTH, length);
}

void wave_maker::Component::loadWave()
{
  auto* hplugin = dynamic_cast<wave_maker::Plugin*>(getHostPlugin());
  // NOTE: the following swap operation invalidates the data held by
  // plugin. It should be clear to the user that after this the
  // wave data held by the plugin should be cleared before use.
  std::swap(wave, hplugin->getWaveData());
  if (wave.empty()) {
    setState(RT::State::PAUSE);
  }
}

void wave_maker::Panel::customizeGUI()
{
  // QGridLayout* customlayout = DefaultGUIModel::getLayout();
  auto* customlayout = dynamic_cast<QVBoxLayout*>(this->layout());
  auto* fileBox = new QGroupBox("File");
  auto* fileButtonLayout = new QHBoxLayout;
  auto* filenameLayout = new QHBoxLayout;
  auto* fileBoxLayout = new QVBoxLayout;
  fileBox->setLayout(fileBoxLayout);
  filenameLayout->addWidget(new QLabel("Filename: "));
  filenameLineEdit->setReadOnly(true);
  filenameLineEdit->setText("No File Loaded");
  filenameLayout->addWidget(filenameLineEdit);
  fileBoxLayout->addLayout(filenameLayout);
  auto* loadBttn = new QPushButton("Load File");
  fileButtonLayout->addWidget(loadBttn);
  auto* previewBttn = new QPushButton("Preview File");
  fileButtonLayout->addWidget(previewBttn);
  fileBoxLayout->addLayout(fileButtonLayout);
  QObject::connect(loadBttn, SIGNAL(clicked()), this, SLOT(loadFile()));
  QObject::connect(previewBttn, SIGNAL(clicked()), this, SLOT(previewFile()));

  customlayout->insertWidget(0, fileBox);
  setLayout(customlayout);
}

void wave_maker::Panel::loadFile()
{
  auto* fd = new QFileDialog(this, "Wave Maker Input File");
  fd->setFileMode(QFileDialog::AnyFile);
  fd->setViewMode(QFileDialog::Detail);
  int64_t length = 0;
  auto* hplugin = dynamic_cast<wave_maker::Plugin*>(getHostPlugin());
  std::vector<double>& wave = hplugin->getWaveData();
  if (fd->exec() == QDialog::Accepted) {
    QStringList files = fd->selectedFiles();
    if (files.isEmpty()) {
      return;
    }
    filename = files.takeFirst();
    filenameLineEdit->setText(filename);
    wave.clear();
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
      QTextStream stream(&file);
      double value = NAN;
      while (!stream.atEnd()) {
        stream >> value;
        wave.push_back(value);
      }
    }
    update_state(RT::State::MODIFY);
  }
}

void wave_maker::Panel::loadFile(QString fileName)
{
  if (fileName == "") {
    return;
  }
  auto* hplugin = dynamic_cast<wave_maker::Plugin*>(getHostPlugin());
  std::vector<double>& wave = hplugin->getWaveData();
  wave.clear();
  QFile file(fileName);
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream stream(&file);
    double value = NAN;
    while (!stream.atEnd()) {
      stream >> value;
      wave.push_back(value);
    }
  }
  update_state(RT::State::MODIFY);
}

void wave_maker::Panel::previewFile()
{
  Event::Object get_period_event(Event::Type::RT_GET_PERIOD_EVENT);
  getRTXIEventManager()->postEvent(&get_period_event);
  auto dt = std::any_cast<int64_t>(get_period_event.getParam("period"));
  std::vector<double> wave;
  QFile file(filename);
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream stream(&file);
    double value = NAN;
    while (!stream.atEnd()) {
      stream >> value;
      wave.push_back(value);
    }
  }
  std::vector<double> time(wave.size());
  std::vector<double> yData(wave.size());
  for (size_t i = 0; i < wave.size(); i++) {
    time[i] = static_cast<double>(dt * i) / RT::OS::SECONDS_TO_NANOSECONDS;
    yData[i] = wave[i];
  }
  auto* preview = new PlotDialog(this,
                                 "Wave Maker Waveform",
                                 time.data(),
                                 yData.data(),
                                 static_cast<int>(wave.size()));
  preview->show();
}

///////// DO NOT MODIFY BELOW //////////
// The exception is if your plugin is not going to need real-time functionality.
// For this case just replace the craeteRTXIComponent return type to nullptr.
// RTXI will automatically handle that case and won't attach a component to the
// real time thread for your plugin.

std::unique_ptr<Widgets::Plugin> createRTXIPlugin(Event::Manager* ev_manager)
{
  return std::make_unique<wave_maker::Plugin>(ev_manager);
}

Widgets::Panel* createRTXIPanel(QMainWindow* main_window,
                                Event::Manager* ev_manager)
{
  return new wave_maker::Panel(main_window, ev_manager);
}

std::unique_ptr<Widgets::Component> createRTXIComponent(
    Widgets::Plugin* host_plugin)
{
  return std::make_unique<wave_maker::Component>(host_plugin);
}

Widgets::FactoryMethods fact;

extern "C"
{
Widgets::FactoryMethods* getFactories()
{
  fact.createPanel = &createRTXIPanel;
  fact.createComponent = &createRTXIComponent;
  fact.createPlugin = &createRTXIPlugin;
  return &fact;
}
};

//////////// END //////////////////////
