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
  GAIN,
  FILE_NAME
};

inline std::vector<Widgets::Variable::Info> get_default_vars()
{
  return {{PARAMETER::LOOP,
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
           1.0},
          {PARAMETER::FILE_NAME,
           "File Name",
           "ASCII Input File",
           Widgets::Variable::COMMENT,
           std::string()}};
}

inline std::vector<IO::channel_t> get_default_channels()
{
  return {{"First Channel Output Name",
           "First Channel Output Description",
           IO::OUTPUT},
          {"First Channel Input Name",
           "First Channel Input Description",
           IO::INPUT}};
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

  // Any functions and data related to the GUI are to be placed here
};

class Component : public Widgets::Component
{
public:
  explicit Component(Widgets::Plugin* hplugin);
  void execute() override;
  void initParameters();

private:
  double dt;
  QString filename;
  size_t idx;
  size_t loop;
  size_t nloops;
  std::vector<double> wave;
  double length;
  double gain;

  // Additional functionality needed for RealTime computation is to be placed
  // here
};

class Plugin : public Widgets::Plugin
{
public:
  explicit Plugin(Event::Manager* ev_manager);
};

}  // namespace wave_maker
