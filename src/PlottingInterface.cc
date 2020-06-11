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

#include <ignition/gui/PlottingInterface.hh>

namespace ignition
{
namespace gui
{

class PlottingIfacePrivate
{
    /// \brief responsible for transport messages and topics
    public: Transport* transport;
};

class TransportPrivate
{
    /// \brief Node for Commincation
    public: ignition::transport::Node node;
    /// \brief topic to subscribe
    public: std::string topic;
    /// \brief msg path to find the plotted msg
    public: std::vector<std::string> fieldFullPath;
    /// \brief y-axis value to plot, double to hold all types
    public: double yData;
};

}
}

using namespace ignition;
using namespace gui;

PlottingInterface :: PlottingInterface() : QObject()
{
    this->dataPtr->transport = new Transport();
    this->InitTimer();
}

PlottingInterface::~PlottingInterface()
{
}

Q_INVOKABLE void PlottingInterface :: setTopic(QString _topic)
{
    this->dataPtr->transport->SetTopic(_topic.toStdString());
}

Q_INVOKABLE void PlottingInterface :: subscribe(QString _topic, QString _fieldPath)
{

    std::vector<std::string> path = ignition::common::Split(_fieldPath.toStdString(),'-');
    this->dataPtr->transport->SetPath(path);
    this->dataPtr->transport->Subscribe(_topic.toStdString());
}

//////////////////////////////////////////
void PlottingInterface::InitTimer()
{
    this->timer = new QTimer();
    this->timer->setInterval(1000);
    connect(this->timer,SIGNAL(timeout()),this,SLOT(UpdateGui()));
    this->timer->start();
}

void PlottingInterface::UpdateGui()
{
    QVariant value = this->dataPtr->transport->GetValue();
    emit plot(1,this->time,value);
    this->time++;
}


Transport :: Transport() : dataPtr(new TransportPrivate)
{
}

////////////////////////////////////////////
Transport :: ~Transport()
{
    this->Unsubscribe();
}

////////////////////////////////////////////
QVariant Transport :: GetValue()
{
    return QVariant(this->dataPtr->yData);
}

////////////////////////////////////////////
void Transport :: SetTopic(const std::string &_topic)
{
    this->dataPtr->topic = _topic;
}

void Transport::SetPath(std::vector<std::string> path)
{
    this->dataPtr->fieldFullPath.clear();
    this->dataPtr->fieldFullPath= path;
}

////////////////////////////////////////////
void Transport :: Unsubscribe()
{
    std::vector<std::string> subscribedTopics = this->dataPtr->node.SubscribedTopics();
    for (unsigned int i = 0; i < subscribedTopics.size(); i++)
    {
        this->dataPtr->node.Unsubscribe(subscribedTopics[i]);
    }
}

////////////////////////////////////////////
void Transport :: Subscribe(std::string _topic)
{
    this->Unsubscribe();
    this->SetTopic(_topic);
    // get the typeMsg of the topic to know which callback to assign
    std::vector<ignition::transport::MessagePublisher> infoMsgs;
    this->dataPtr->node.TopicInfo(this->dataPtr->topic, infoMsgs);
    std::string msgType = infoMsgs[0].MsgTypeName();

    this->dataPtr->node.Subscribe(this->dataPtr->topic, &Transport::Callback, this);
}

void Transport::Callback(const google::protobuf::Message &_msg)
{
    auto msgDescriptor = _msg.GetDescriptor();
    auto ref = _msg.GetReflection();

    google::protobuf::Message *valueMsg = nullptr;

    int pathSize = this->dataPtr->fieldFullPath.size();

    // loop until you reach the last field in the path (the field that we want its value to plot)
    for (int i = 0; i < pathSize-1 ; i++)
    {
        std::string fieldName = this->dataPtr->fieldFullPath[i];

        auto field = msgDescriptor->FindFieldByName(fieldName);

        msgDescriptor = field->message_type();

        if (valueMsg)
            valueMsg = ref->MutableMessage(const_cast<google::protobuf::Message *>(valueMsg) ,field);
        else
            valueMsg = ref->MutableMessage(const_cast<google::protobuf::Message *>(&_msg) ,field);

        ref = valueMsg->GetReflection();
    }


    std::string fieldName = this->dataPtr->fieldFullPath[pathSize-1];
    double data;

    if (valueMsg)
    {
        auto field = valueMsg->GetDescriptor()->FindFieldByName(fieldName);
        data = this->GetPlotData(*valueMsg, field);
    }
    else
    {
        auto field = msgDescriptor->FindFieldByName(fieldName);
        data = this->GetPlotData(_msg, field);
    }

    this->dataPtr->yData = data;
    igndbg << "plotData= " << data << std :: endl;
}

double Transport::GetPlotData(const google::protobuf::Message &_msg, const google::protobuf::FieldDescriptor *field)
{
    using namespace google::protobuf;
    auto ref = _msg.GetReflection();
    auto type = field->type();

    if (type == FieldDescriptor::Type::TYPE_DOUBLE)
        return ref->GetDouble(_msg,field);
    else if (type == FieldDescriptor::Type::TYPE_FLOAT)
        return ref->GetFloat(_msg,field);
    else if (type == FieldDescriptor::Type::TYPE_INT32)
        return ref->GetInt32(_msg,field);
    else if (type == FieldDescriptor::Type::TYPE_INT64)
        return ref->GetInt64(_msg,field);
    else if (type == FieldDescriptor::Type::TYPE_BOOL)
        return ref->GetBool(_msg,field);
    else if (type == FieldDescriptor::Type::TYPE_UINT32)
        return ref->GetUInt32(_msg,field);
    else if (type == FieldDescriptor::Type::TYPE_UINT64)
        return ref->GetUInt64(_msg,field);
    else
    {
        ignerr << "Non Plotting Type" << std::endl;
        return 0;
    }
}
