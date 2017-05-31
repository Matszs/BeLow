var dataset;
var selectBox = document.getElementById("id_selector");

function hex2a(hexx) {
    var hex = hexx.toString();//force conversion
    var str = '';
    for (var i = 0; i < hex.length; i += 2)
        str += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
    return str;
}

var config = {
    apiKey: "AIzaSyB2skAeJNAiq4JjcJeU3Sx9rOJQPWmcv6Y",
    authDomain: "below-e60f0.firebaseapp.com",
    databaseURL: "https://below-e60f0.firebaseio.com/",
    storageBucket: "below-e60f0.appspot.com"
};

firebase.initializeApp(config);
var database = firebase.database();

// database.ref('/data').then(function(snapshot) {
//     console.log(snapshot);
// });

firebase.database().ref('/data').once('value').then(function(snapshot) {
    var firebaseData = snapshot.val();
    dataset = {};
    selectData = "";

    var i = 0;
    for(var key in firebaseData) {
        var dataPacket = firebaseData[key]['DevEUI_uplink'];

        if(typeof(dataPacket) == 'undefined')
            continue;

        var payload = hex2a(dataPacket['payload_hex']);
        var dataSplit = payload.split('|', 3);

        var deviceId = dataSplit[0];
        var major = parseInt(dataSplit[1]);
        var minor = ((typeof(dataSplit[2]) !== 'undefined') ? parseInt(dataSplit[2]) : -1);

        if(typeof(dataset[deviceId]) === 'undefined') {
            dataset[deviceId] = [];
            selectData += "<option value='" + deviceId + "'>" + deviceId + "</option>";
        }

        //dataset[deviceId].push([dataset[deviceId].length, major, minor]);
        dataset[deviceId].push([dataset[deviceId].length, major]);


        if(deviceId == "00001338B64445208F0C720E1742F0A1" && major > 1300 && major < 4500) {
            dataArr.push([i, major]);
            i++;
        }
    }

    selectBox.innerHTML = selectData;
});

google.charts.load('current', {'packages':['line']});

function draw(id) {
    var options = {
      chart: {
        title: 'Temperatuur verloop',
        subtitle: 'in graden celsius'
      },
      width: 1800,
      height: 900
    };

    var data = new google.visualization.DataTable();
    data.addColumn('number', 'Datum');
    data.addColumn('number', 'Temperatuur');

    data.addRows(dataset[id]);

    var chart = new google.charts.Line(document.getElementById('linechart_material'));
    chart.draw(data, google.charts.Line.convertOptions(options));
}

selectBox.addEventListener("change", function() {
    var id = selectBox.value;

    draw(id);
}); 