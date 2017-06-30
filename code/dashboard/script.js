var myFirebaseRef = new Firebase("https://below-e60f0.firebaseio.com/");
var dataRef = myFirebaseRef.child('data');

function hex2a(hexx) {
    var hex = hexx.toString();//force conversion
    var str = '';
    for (var i = 0; i < hex.length; i += 2)
        str += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
    return str;
}

var doorTimer;

var temperatureChart;
var temperatureChartData;
var tempI = 0;

var doorChart;
var doorChartData;
var doorI = 0;

var lightChart;
var lightChartData;
var lightI = 0;

var options = {
    title: null,
    width: 400,
    height: 400,
    animation:{
        duration: 1000,
        easing: 'out'
    },
    legend: 'none'
};

var optionsDoor = {
    title: null,
    width: 400,
    height: 400,
    animation:{
        duration: 1000,
        easing: 'out'
    },
    legend: 'none',
    vAxis: {
        minValue:0,
        maxValue:1,
        ticks: []
    },
    colors: ['#6e20ff'],
    bar: {groupWidth: "95%"}
};

// onload callback
function drawChart() {
    // temperatuur ding
    temperatureChartData = new google.visualization.DataTable();

    temperatureChartData.addColumn('datetime', 'Time');
    temperatureChartData.addColumn('number', 'Temp');

    temperatureChart = new google.visualization.LineChart($('#temperature-graph').get(0));
    temperatureChart.draw(temperatureChartData, options);

    // deur ding
    doorChartData = new google.visualization.DataTable();

    doorChartData.addColumn('datetime', 'Time');
    doorChartData.addColumn('number', 'Open/dicht', 'stroke-width: 24;');

    doorChart = new google.visualization.ColumnChart($('#door-graph').get(0));
    doorChart.draw(doorChartData, optionsDoor);

    // licht ding
    lightChartData = new google.visualization.DataTable();

    lightChartData.addColumn('datetime', 'Time');
    lightChartData.addColumn('number', 'LDR value');

    lightChart = new google.visualization.LineChart($('#light-graph').get(0));
    lightChart.draw(lightChartData, options);

}

// load chart lib
google.load('visualization', '1', {
    packages: ['corechart']
});

// call drawChart once google charts is loaded
google.setOnLoadCallback(drawChart);



dataRef.on("child_added", function(snapshot) {

    var child = snapshot.val();
    var data = hex2a(child['DevEUI_uplink']['payload_hex']);
    var timeThing = child['DevEUI_uplink']['Time'];
    var dataSplit = data.split('|', 3);

    var time = (new Date()).getTime();

    if(dataSplit[2] == 2) {

        temperatureChartData.addRow([new Date(timeThing), (parseFloat(dataSplit[1]) / 100)]);
        if(tempI > 5)
            temperatureChartData.removeRow(0);

        temperatureChart.draw(temperatureChartData, options);
        tempI++;

    } else if(dataSplit[2] == 1) {

        doorChartData.addRow([new Date(timeThing), 1]);
        if(doorI > 5)
            doorChartData.removeRow(0);
        doorChart.draw(doorChartData, optionsDoor);
        doorI++;

        if(doorTimer != null)
            clearTimeout(doorTimer);

        doorTimer = setTimeout(function() {
            doorChartData.addRow([new Date(), 0]);
            doorChart.draw(doorChartData, optionsDoor);
        }, 30000);

    } else if(dataSplit[2] == 3) {

        lightChartData.addRow([new Date(timeThing), (parseFloat(dataSplit[1]))]);
        if(lightI > 5)
            lightChartData.removeRow(0);
        lightChart.draw(lightChartData, options);
        lightI++;

    }

});