/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#ifndef IGNITION_GUI_PLOTTINGINTERFACE_HH_
#define IGNITION_GUI_PLOTTINGINTERFACE_HH_

#include <QObject>
#include <QTimer>
#include <QString>
#include <QVariant>
#include <bits/stdc++.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <ignition/transport.hh>
#include <ignition/transport/Node.hh>
#include <ignition/transport/MessageInfo.hh>
#include <ignition/transport/Publisher.hh>
#include <ignition/gui/Application.hh>
#include "ignition/gui/Export.hh"
#include <ignition/common.hh>
#include <ignition/gui.hh>
#include <memory>

namespace ignition
{
namespace gui
{

class TransportPrivate;

/// \brief handle Transport Topics Subscribing for one object(Chart) (will extend)
class Transport
{
    private: std::unique_ptr<TransportPrivate> dataPtr;

    /// \brief Constructor
    public: Transport();

    /// \brief Destructor
    public: ~Transport();

    /// \brief get the published value according to the topic's msg type
    /// wrap it in QVariant to send to javascript
    public: QVariant GetValue();

    /// \brief set the topic to register
    public: void SetTopic(const std::string &_topic);

    public: void SetPath(std::vector<std::string> path);

    /// \brief unsubscribe from the subscribed topics
    public: void Unsubscribe();

    /// \brief subscribe to the topic in dataPtr->topic
    public: void Subscribe(std::string _topic);

    /// \brief callback for subscribing to a topic
    public: void Callback(const google::protobuf::Message &_msg);

    /// \brief check the plotable types and get data from reflection
    public: double GetPlotData(const google::protobuf::Message &_msg,
                           const google::protobuf::FieldDescriptor *field);

};

class PlottingIfacePrivate;

class PlottingInterface : public QObject
{
    Q_OBJECT

    /// \brief current plotting time
    float time;
    /// \brief timer to control the interval of the next plotting
    QTimer* timer;
    /// \brief configration of the timer
    void InitTimer();


    /// \brief update the plotting
    public slots: void UpdateGui();

    /// \brief Constructor
    public: explicit PlottingInterface();

    /// \brief Destructor
    public: ~PlottingInterface();

    /// \brief set a topic to subscribe to it (just used for testing);
    public: Q_INVOKABLE void setTopic(QString _topic);

    /// \brief param[in] _index subscribe to the selected item in the Topics Tree which has index _index
    public: Q_INVOKABLE void subscribe(QString _topic, QString _fieldPath);

    signals: void plot(int _chart, float _x, QVariant _y);

    private: std::unique_ptr<PlottingIfacePrivate> dataPtr;
};

}
}
#endif
