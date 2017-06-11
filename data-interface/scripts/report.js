
var seriesOptions = [];

function loadXMLDoc(file) {
   var xhttp = new XMLHttpRequest();
   var res;
   xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
      res = this.responseText;
      }
   };
   xhttp.open("GET", file, false);
   xhttp.send();
   return res;
}

function loadData(file, lables) {
   var arrtime = loadXMLDoc(file[file.length-1]).split(/[,,]/).map(Number);
   console.log(file[file.length-1]+":"+(arrtime.length-1));
   for (var i = 0; i < lables.length; i++) {
      var arrValue = loadXMLDoc(file[i]).split(/[,,]/).map(Number);
      var arrSize = arrValue.length - 1; // ignore ',' at last
      console.log(file[i]+":"+arrSize);
      seriesOptions[i] = {
         name : lables[i],
         data: (function () {
           // generate an array of random data
            var data = [];
            for (var j = 0; j < arrSize; j++) {
               data.push([arrtime[j]*1000, arrValue[j]]);
            }
            return data;
         }())
      }
      // console.log(seriesOptions[i].data);
   }
}

function createChartTotal(idName, file, howMany, unit) {

   var chartTotal = Highcharts.stockChart(idName, {

      chart: {
         height: 350,
         type: 'spline'
      },
      
      rangeSelector: {
         buttons: [{
             count: 1,
             type: 'minute',
             text: '1m'
         }, {
             count: 5,
             type: 'minute',
             text: '5m'
         },{
             count: 10,
             type: 'minute',
             text: '10m'
         }, {
             type: 'all',
             text: 'All'
         }],
         inputEnabled: false,
         selected: false
      },
      
      yAxis: {
         labels: {
            formatter: function () {
               return this.value + unit;
            }
         }
      },
      
      tooltip: {
         pointFormat: '<span style="color:{series.color}">{series.name}</span>: <b>{point.y}</b><br/>',
         split: false
      },

      series: seriesOptions.slice(0, howMany)
    });

}

function creatOneLine(idName, file, indexFile, unit) {

   var chartBox = Highcharts.stockChart(idName, {
      
      chart: {
         height: 350
      },

      rangeSelector: {
         buttons: [{
             count: 1,
             type: 'minute',
             text: '1m'
         }, {
             count: 5,
             type: 'minute',
             text: '5m'
         },{
             count: 10,
             type: 'minute',
             text: '10m'
         }, {
             type: 'all',
             text: 'All'
         }],
         inputEnabled: false,
         selected: false
      },

      plotOptions: {
         line: {
            dataLabels: {
            enabled: true
         },
         enableMouseTracking: false
         }
      },
      
      yAxis: {
         labels: {
            formatter: function () {
               return this.value + unit;
            }
         }
      },

      series: [{
         name: idName,
         data: seriesOptions[indexFile].data
      }]

   });

}

