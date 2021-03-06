/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <iostream>

#include <ignition/common/Console.hh>

#ifndef Q_MOC_RUN
  #include <ignition/gui/qt.h>
  #include <ignition/gui/Application.hh>
  #include <ignition/gui/Dialog.hh>
#endif

//////////////////////////////////////////////////
int main(int _argc, char **_argv)
{
  std::cout << "Hello, GUI!" << std::endl;

  // Increase verboosity so we see all messages
  ignition::common::Console::SetVerbosity(4);

  // Initialize app
  ignition::gui::Application app(_argc, _argv,
      ignition::gui::WindowType::kDialog);

  // Load plugins / config
  app.LoadPlugin("Publisher");

  // TODO(chapulina): fix multiple dialogs
  // app.LoadPlugin("TopicEcho");

  // Run dialogs
  app.exec();

  std::cout << "After run" << std::endl;

  return 0;
}

