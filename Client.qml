import QtQuick 2.0
import com.company.client 1.0
Item {

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
        {

           chatContent.append({content:sender+" : "+message})
        }
    }

}
