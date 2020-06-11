import QtQuick 2.0
import QtCharts 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
GridLayout
{
    Layout.minimumWidth: 720
    Layout.minimumHeight: 400
    rows: 2
    columns: 4
    rowSpacing: 0; columnSpacing: 0;

    anchors.fill: parent

    Rectangle{
        Layout.row: 0
        Layout.column: 0
        Layout.fillWidth: true
        Layout.columnSpan: 4
        Layout.rowSpan: 1
        Layout.preferredHeight: 50
        Layout.preferredWidth: parent.width

        id : subscribe_topic

        TextField {
            width: parent.width
            height: parent.height/2
            opacity: 0.8
            onAccepted : PlottingInterface.setTopic(this.text);
        }
        TextField {
            width: parent.width
            height: parent.height/2
            opacity: 0.8
            onAccepted : PlottingInterface.setTopic(this.text);
        }
    }

    ChartView {

        Layout.row: 1
        Layout.column: 0
        Layout.columnSpan: 4
        Layout.fillHeight: true
        Layout.fillWidth:  true

        id : view
        antialiasing: true
        opacity: 1
        title: "Ignition GUI"
        titleColor: "white"
        // animations
        animationDuration: 400
        animationOptions: ChartView.SeriesAnimations
        theme: ChartView.ChartThemeQt

        MouseArea {
            id:areaView
            anchors.fill:parent
            hoverEnabled: true
            property bool flag: false
            onEntered: {
                if(flag)
                {
                    // TODO : draw a vertical line and show the value of its coresponding x value at the mouse
                    flag=false;
                }
            }
        }

        // initial animation
        Component.onCompleted:{initialAnimation.start();
            setAxisX(horizontalAxis,s); setAxisY(verticalAxis,s);
            setAxisX(horizontalAxis,transport); setAxisY(verticalAxis,transport);
        }
        NumberAnimation {
            id: initialAnimation
            running: false
            target: view
            property: "scale"
            duration: 1200
            easing.type: Easing.OutBounce
            from:0 ; to:1;
        }

        ValueAxis {
            id : verticalAxis
            min: 0;
            max: 3;
            tickCount: 9
        }

        ValueAxis {
            id : horizontalAxis
            min: 0
            max: 1
            tickCount: 9
        }

        LineSeries {
            id :s
            name: "Ignition GUI"
            width: 1;
            color: "blue"
        }
        LineSeries {
            id : transport
            name: "Ignition transport"
            width: 1;
            color: "red"
        }

        Connections {
            target: PlottingInterface
            onPlot : {
                view.appendPoint(_x,_y,_chart);
            }
        }

        function appendPoint(_x,_y,_chart){
            console.log("(",_x,",",_y,")");
            if (horizontalAxis.max -5 < _x)
                horizontalAxis.max =_x +5 ;
            if (verticalAxis.max -3 < _y )
                verticalAxis.max = _y + 3 ;

            if(_chart === 0 )
                s.append(_x,_y);
            else if ( _chart === 1)
                transport.append(_x,_y);

        }
    }
}
