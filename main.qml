import QtQuick 2.12
import QtQuick.Controls 2.12
import com.company.server 1.0
import com.company.client 1.0
ApplicationWindow {
    visible: true
    width: 620
    height: 420
    property bool isConnected: false
    property string connectName:""
    property int mode
    property bool isDiscovered: false
    signal sendMessage(string value)
    id:top
    Server
    {
        id:server
        //onStartChanged:abc.text="server running"

        onClientConnected:
        {
            isConnected=true;
            connectName=name;
            info.text="";
        }
        onMessageReceived:
        {

           chatContent.append({content:sender+" : "+message})
        }
        onClientDiscon:
        {

            isConnected=false;
        }
        onServerStopped:
        {
            isConnected=false
        }

    }
    Client
    {
        id:client
        onConnected:
        {
            isConnected=true
            connectName=name
        }
        onDiscovered:
        {
            isDiscovered=true
            it.clear();
            info.text=""
           var items=getList();
            console.log(items);

            for(var i=0;i<items.length;i++)
            {
                it.append({num:i+1,name:items[i]});
            }
        }
        onNoService:
        {
            info.text="no service"
        }

        onMessageReceived:
        {console.log("msg re")
            console.log(sender+" : "+message)
           chatContent.append({content:sender+" : "+message})
        }
        onClientStopped:
        {
            isConnected=false
        }
    }

    Button
    {
        id:connectBtn
        text: "connect"
        //anchors.horizontalCenter: parent.horizontalCenter
        onClicked:
        {
              ask.visible=true;
            //server.startServer()
        }
        background : Rectangle
        {
            color:"green"
        }
    }
Rectangle
{
    radius:7

    id:ask
    anchors.centerIn: parent
    width:host.width
    height:host.height*3
    Button
    {
        id:host
        anchors.top: parent.top
        text: "host"
        background: Rectangle
        {
            color:"green"
        }
        onClicked:
        {
              ask.visible=false;
            server.startServer()
            mode=0;
            info.text="waiting for connection"
        }
    }
    Button
    {

        anchors.bottom:parent.bottom
        text: "join"
        background: Rectangle
        {
            color:"purple"
        }
        onClicked:
        {
              ask.visible=false;
            client.clientMode()
            mode=1;
             info.text="waiting for connection"
        }
    }
    visible:false
}

   Rectangle
   {
       id:connectInfo
    radius:5
    border.color: isConnected?"green":"red"
    anchors.left: connectBtn.right
    anchors.right: parent.right
    width:parent.width-connectBtn.width
    height:name.height*1.5

    Text {
        anchors.centerIn: parent
        id: name
        text: isConnected?"connected with "+connectName:"not connected"
    }


   }

   Rectangle
   {
       id:textf
       anchors.bottom:parent.bottom
       radius:5
       width:connectInfo.width
       height:connectInfo.height*2
       border.color: "green"
       color:"white"
       smooth: true
       clip: true
   TextField
   {

       id:boom
      anchors.fill:parent
      anchors.margins:2

   }
   }
   Button{
       id:send
       text:"send"
       anchors.bottom:parent.bottom
       anchors.left:textf.right
       width:parent.width-textf.width
       height:boom.height
       enabled: isConnected
       onClicked:
       {
          //server.sendmsg=sendmsg
           if(mode===0)
           {
           server.sendMessage(boom.text)
           }
           else if(mode===1)
           {
               console.log("client msg")
           client.sendMessage(boom.text)
               }
           chatContent.append({content:"Me : "+boom.text})
           boom.text=""
       }
   }
   Button{
       id:disconnectBtn
       text:"disconnect"
        onClicked:
       {
            isConnected=false
            if(mode===0)
            {
                console.log("disc server")
            server.stopServer()
            }
            else if(mode===1)
            {
                console.log("client msg")
                client.stopClient()
                }
       }
        background: Rectangle
        {
            color:"red"
        }
        visible: isConnected
   }

   ListModel {
       id: chatContent
       ListElement {
           content: "Connected"
       }
   }

   ListView {
       visible: isConnected
       id: chatView
       width: top.width-5
       height: top.height-(connectBtn.height+send.height)
       anchors.top: connectBtn.bottom
       anchors.left: top.left
       anchors.right: top.right
       anchors.bottom: send.top
       anchors.margins: 3
       model: chatContent
       clip: true
       delegate: Component {
           Text {
               text: modelData
           }
       }
   }

   Component {
           id: listDelegate
           Item {
               width: 200; height: 50
               Text { text: num }
               Text { text: name; anchors.right: parent.right }

               // Double the price when clicked.
               MouseArea {
                   anchors.fill: parent
                   onClicked: {
                       client.sendIndex(num);
                       it.clear();
                       isDiscovered=false
                   }
               }
           }
       }

   ListModel
   {
       id:it
       ListElement
       {
           num:0
           name:"waiting for connection"
       }
   }

   ListView
   {
       width: 180; height: 200
       model:it
       anchors.top: connectBtn.bottom
       delegate:listDelegate
       visible: isDiscovered
       spacing: 2

   }

   Text {
       id: info
       text:""
        anchors.top: connectBtn.bottom
   }

}
