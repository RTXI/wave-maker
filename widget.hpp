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

#include <string>
#include <rtxi/plot/basicplot.h>
#include <rtxi/plot/plotdialog.h>
#include <rtxi/widgets.hpp>

// This is an generated header file. You may change the namespace, but
// make sure to do the same in implementation (.cpp) file
namespace wave_maker
{

constexpr std::string_view MODULE_NAME = "wave-maker";

enum PARAMETER : Widgets::Variable::Id
{
  // set parameter ids here
  LOOP = 0,
  LENGTH,
  GAIN
};

inline std::vector<Widgets::Variable::Info> get_default_vars()
{
  return {
      {PARAMETER::LOOP,
       "Loops",
       "Number of Times to Loop Data From File",
       Widgets::Variable::UINT_PARAMETER,
       uint64_t {1}},
      {PARAMETER::LENGTH,
       "Length (s)",
       "Length of Trial is Computed From the Real-Time Period",
       Widgets::Variable::INT_PARAMETER,
       int64_t {1}},
      {PARAMETER::GAIN,
       "Gain",
       "Factor to Amplify the Signal",
       Widgets::Variable::DOUBLE_PARAMETER,
       1.0}
  };
}

inline std::vector<IO::channel_t> get_default_channels()
{
  return {{"Output", "Signal from File", IO::OUTPUT}};
}

class Panel : public Widgets::Panel
{
  Q_OBJECT
public:
  Panel(QMainWindow* main_window, Event::Manager* ev_manager);
  void customizeGUI();

private slots:
  void loadFile();
  void loadFile(QString);
  void previewFile();

private:
  QString filename;
  QLineEdit* filenameLineEdit=nullptr;
};

class Component : public Widgets::Component
{
public:
  explicit Component(Widgets::Plugin* hplugin);
  void execute() override;
  void initParameters();
  void updateParameters();

private:
  void loadWave();
  int64_t dt; // period in nanoseconds
  size_t idx;
  size_t loop;
  uint64_t nloops;
  std::vector<double> wave;
  int64_t length;
  double gain;
};

class Plugin : public Widgets::Plugin
{
public:
  explicit Plugin(Event::Manager* ev_manager);
  std::vector<double>& getWaveData(){ return wave_data; }

private:
  std::vector<double> wave_data;
};

}  // namespace wave_maker
