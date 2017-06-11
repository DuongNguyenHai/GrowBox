
window.onload = function() {

var seriesOptions = [];
var names = ['COLD', 'BOX', 'HOT'];
var fileLog = ["time.txt", "ds18b_cold.txt", "dht.txt", "ds18b_hot.txt"];

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

var arrtime = loadXMLDoc(fileLog[0]).split(/[,,]/).map(Number);

function loadData(argument) {

  console.log(fileLog[0]+":"+(arrtime.length-1));
  for (var i = 0; i < names.length; i++) {
     var arrtempt = loadXMLDoc(fileLog[i+1]).split(/[,,]/).map(Number);
     var arrSize = arrtempt.length - 1; // ignore ',' at last
     console.log(fileLog[i+1]+":"+arrSize);
     seriesOptions[i] = {
        name : names[i],
        data: (function () {
           // generate an array of random data
           var data = [];
           for (var j = 0; j < arrSize; j++) {
              data.push([arrtime[j]*1000, arrtempt[j]]);
           }
           return data;
         }())
     }
     // console.log(seriesOptions[i].data);
  }
}

loadData();


function createChartTotal() {

   var chartTotal = Highcharts.stockChart('total', {

      chart: {
         height: 350,
         type: 'spline',
            events: {
                load: function () {
                    // set up the updating of the chart each second
                    var ser = this.series;
                    setInterval(function () {
                      var arrtime = loadXMLDoc(fileLog[0]).split(/[,,]/).map(Number);
                      // console.log(fileLog[0]+":"+(arrtime.length-1));
                        for (var i = 0; i < names.length; i++) {
                           var arrtempt = loadXMLDoc(fileLog[i+1]).split(/[,,]/).map(Number);
                           var arrSize = arrtempt.length - 1; // ignore ',' at last
                           ser[i].addPoint([arrtime[arrSize-1]*1000, arrtempt[arrSize-1]], true, true);
                           // console.log(arrSize +":"+arrtime[arrSize-1] + "," + arrtempt[arrSize-1]);
                           // console.log(fileLog[0]+":"+(arrtime.length-1));
                        }

                    }, 10000);
                }
            }
      },
      
      rangeSelector: {
         buttons: [{
             count: 1,
             type: 'minute',
             text: '1M'
         }, {
             count: 5,
             type: 'minute',
             text: '5M'
         }, {
             type: 'all',
             text: 'All'
         }],
         inputEnabled: false,
         selected: 2
      },
      
      yAxis: {
            labels: {
                formatter: function () {
                    return this.value + '°C';
                }
            }
      },
        tooltip: {
            pointFormat: '<span style="color:{series.color}">{series.name}</span>: <b>{point.y}</b><br/>',
            split: false
        },

      series: seriesOptions
    });

   // chartTotal.setOptions({
   //    chart: {
   //       style: {
   //          fontFamily: 'serif'
   //       }
   //    }
   // });
}

createChartTotal();

function creatOneLine(idName, indexFile) {

// Create the chart
   var chartBox = Highcharts.stockChart(idName, {
      
      chart: {
         height: 350,
         events: {
            load: function () {

                // set up the updating of the chart each second
                var series = this.series[0];
                setInterval(function () {
                   var arrtime = loadXMLDoc(fileLog[0]).split(/[,,]/).map(Number);
                   var arrtempt = loadXMLDoc(fileLog[indexFile]).split(/[,,]/).map(Number);
                  var arrSize = arrtempt.length - 1; // ignore ',' at last
                  series.addPoint([arrtime[arrSize-1]*1000, arrtempt[arrSize-1]], true, true);
                  // console.log(arrtime[arrSize-2] + "," + arrtempt[arrSize-1]);
                }, 10000);
            }
        }
      },

      rangeSelector: {
           buttons: [{
               count: 1,
               type: 'minute',
               text: '1M'
           }, {
               count: 5,
               type: 'minute',
               text: '5M'
           }, {
               type: 'all',
               text: 'All'
           }],
           inputEnabled: false,
           selected: 1
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
                    return this.value + '°C';
                }
            }
      },

      series: [{
         name: 'Box',
         data: (function () {
            var data = [];

            var arrtempt = loadXMLDoc(fileLog[indexFile]).split(/[,,]/).map(Number);
            for (var j = 0; j < arrtempt.length - 1; j++) {
               var arry = [arrtime[j]*1000,arrtempt[j]];
               data.push(arry);
            }
            return data;
        }())

      }]

   });

}

creatOneLine("inBox", 2);
creatOneLine("hotSide", 3);
creatOneLine("coldSide", 1);


};
