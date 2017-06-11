$(document).ready(function() {

// 
var setup = "setup";
var logTempt = "logTempt";
var logHumidity = "logHumidity";
var logLamp = "logLamp";
var logProgressTemperature = "logProgressTemperature";
var logProgressHumidity = "logProgressHumidity";

// check if device is touch screen
var TOUCHSCREEN = ('ontouchstart' in window) || (navigator.msMaxTouchPoints > 0);

var seriesOptions = [
    {
        name: "tempt come in",
        collection: logProgressTemperature,
        property: "temptIn"
    },
    {
        name: "tempt in box",
        collection: logProgressTemperature,
        property: "temptInBox"
    },
    {
        name: "tempt come out",
        collection: logProgressTemperature,
        property: "temptOut"
    }
]

function hour2second(hh) {
  return hh*3600;
}

function minute2second(mm) {
  return mm*60;
}

$(".switchData").on("click", function() {

    if($(this).hasClass("sw-type-enable")) {
        $(this).toggleClass("switch-on");
        var sw = $(this).closest(".set-element").find(".sw-type-state");
        if($(this).hasClass("switch-on"))
           $(sw).removeClass("enable");
        else
            $(sw).addClass("enable");
    } else if($(this).hasClass("sw-type-state")) {
        $(this).toggleClass("switch-on");
    } else {}

	var value;
	// var objectName = $(this).parent().children(".switch-name").attr("object");
	// var type = $(this).parent().children(".switch-name").attr("type");

    var object = $(this).closest(".request-area");
    var objectName = $(object).attr("object");
    var type = $(object).attr("type");

	if($(this).hasClass("switch-on"))
		value = true;
	else
		value = false;

	requestSetup($(this), objectName, setup, type, value);
});

$(".sendData").on("click", function() {

    var object = $(this).closest(".request-area");
    var objectName = $(object).attr("object");
    var type = $(object).attr("type");

	if(objectName=="lamp") {

        var start = $("input[name=lightOnFrom]").val();
        var end = $("input[name=lightOnTo]").val();
        if( start<0 || start>24 || end<0 || end>24 || start==end ) {
            alertHelper("D: Vui lòng nhập thời gian theo giờ, trong khoảng từ 0 tới 24h. "+ "</br>" +"ví dụ: Bật đèn từ 7 tới 17h. </br>Lưu ý thời gian bật và tắt phải là hai giờ khác nhau.");
            return 0;
        }
        if(start==0) start = 24;
        if(end==0) end = 24;
		var data = hour2second(start) + "," + hour2second(end);

	} else if(objectName=="temptAndHumy") {

        var raw = $(object).find("input").val();
        if(type=="temptInBox") {
            if( raw<15 || raw>40 ) {
               alertHelper("D: Nhiệt độ chỉ có thể nằm trong khoảng từ 15 - 40 °C");
               return 0;
            }
            $('input[reference=setTempt]').val(raw);
            var data = raw;
        } 
        else if(type=="humidity") {
            if( raw<50 || raw>100 ) {
               alertHelper("D: Độ ẩm chỉ có thể nằm trong khoảng từ 50% - 100%");
               return 0;
            }
            $('input[reference=setHumy]').val(raw);
            var data = raw;
        }
        else {
            var nums = getDistinctArray(raw.split(/[,,]/).map(Number));  // spilit ',' to array -> convert to number array and discard the same value   
            for (var i = 0; i < nums.length; i++) {
              if( !nums[i] || nums[i] < 0 || nums[i] > 24 ) {
                alertHelper("D: Vui lòng nhập chuỗi thời gian theo giờ từ 0 tới 24h và được ngăn cách nhau bằng dấu phẩy."+ "</br>" +"ví dụ: 2, 5, 7");
                return 0;
              }
            }
            nums.sort(function(a, b){return a-b});
            var data = nums.map(hour2second).toString();
        }
	} else if(objectName="ventilation") {

        var opentime = object.find("input[name=venInvervalOpen]").val();
        var closeTime = object.find("input[name=venInvervalInterval]").val();
        if( opentime<0 || opentime>Math.pow(2, 32) || closeTime<0 || closeTime>Math.pow(2, 32) ) {
            alertHelper("D: Vui lòng nhập thời gian theo phút và thời gian mở đóng cửa trong khoảng từ 0 tới 2^32 phút."+ "</br>" +"ví dụ: Mở 2 phút đóng 30 phút");
            return 0;
        }
        var data = minute2second(opentime) + "," + minute2second(closeTime);
	}
	
	requestSetup($(this), objectName, setup, type, data);
});

$(".hand-reading").on("click", function() {
    var object = $(this).closest(".request-area");
    var objectName = $(object).attr("object");
    var type = $(object).attr("type");
    requestSetup($(this), objectName, setup, type, true);
});

$(".enable-prop").on("click", function() {
    var object = $(this).closest(".request-area");
    var objectName = $(object).attr("object");
    var type = $(this).attr("type");
    var ena = true;
    if($(this).hasClass("enable")) {
        ena = false;
    }
    $(this).toggleClass("enable");
    requestSetup($(this), objectName, setup, type, ena);
});

// draw graph of temperature and humidity in box
drawGraph("graphTemptInBox", "nhiệt độ", "line", logTempt, "temptInBox", " °C");
drawGraph("graphHumyInBox", "độ ẩm", "area", logHumidity, "humidity", "%");
// drawGraph("graphLamp", "anh sang", "area", logLamp, "lightLevel", " lux");
// 
drawGraph("graphHumyProgress", "độ ẩm", "line", logProgressHumidity, "humidity", " %");
// draw progress of setting temperature
var multiChart = drawMultiChart("graphTemptProgress", seriesOptions, "°C");

$(".adv-option").on("click", function() {
    $(".adv-option-content").toggleClass("showup");
    $(this).toggleClass("btn-primary");
});

// update status of all devices
updateStatus( $(".request-area[object=lamp]").find(".switchData.sw-type-state"), "lamp", "state");
updateStatus( $(".request-area[object=ventilation]").find(".switchData.sw-type-state"),"ventilation", "state");
// show system time
var epochSeconds = $("#infor-systemTime").find(".infor-val").attr("epoch-time");
countTime(epochSeconds*1000 + 1000);  // plus 1 second to compensate time elapsed

// update info of object to server
function requestSetup(that, objName, resq, type, val) {
    console.log(objName, type, val);
    $.post(
        "function/request.php",
        {
            request : resq,
            name : objName,
            type: type,
            value : val
        }
    ).done(function( respone ) {
        // console.log(data);
        var res = alertHelper(respone);
        if(res==-1){
            setTimeout(function() {
                $(".switchData").removeClass("enable");
                $(".sendData").removeClass("enable");
                $(".hand-reading").removeClass("enable");
                // changeSwitchData($(that));
                if($(that).hasClass("sw-type-enable")) {
                    $(that).toggleClass("switch-on");
                }
                alertSystem("alert-danger", "Không thể kết nối tới trung tâm điều khiển.", "Yêu cầu kiểm tra log và khởi động lại hệ thống");
            }, 2000);

        }
    });
}

function requestForChart(coll, type, property, idx = 0) {
	return $.post(
		"function/request.php",
		{
			request: "log",
			collection: coll,
      		type: type,
            property: property,
      		index: idx
		}
	);
}

function loadData(chart, collection, property) {
  	var res = requestForChart(collection, "total", property);
  	res.success(function (data) {
		// console.log(data);
		var cont = JSON.parse(data);
		var point = [];
		for(i in cont) {
			point.push([Number(cont[i][0]), round2decimal(cont[i][1])]);
		}
		chart.update({
	        series: [{
		        data: point
		    }]
    	});
  	});
}

function drawGraph(idName, lineName, chartType, collection, property, unit) {

   	var chartBox = Highcharts.stockChart(idName, {
      
      	chart: {
         	height: 350,
            type: 'spline',
         	events: {
	            load: function() {
	            	var ser = this.series[0];
	            	setInterval(function () {
                        // console.log(collection, ser.data.length);
	            		var res = requestForChart(collection, "short", property, ser.data.length);
						res.success(function(data) {
							// console.log(data);
							if(data.length>0) {
								var cont = JSON.parse(data);
								for(j in cont) {
                                    ser.addPoint([Number(cont[j][0]), round2decimal(cont[j][1])], true, false);
								}
							} else {
								// console.log(collection, ": no new data");
							}
							
						});
	            	}, 5000);
	            }
         	}
      	},

		rangeSelector: {
		 	buttons: [{
		    count: 1,
		    type: 'minute',
		    text: '1m'
		}, 	{
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
        xAxis: {
            type: 'datetime',
            dateTimeLabelFormats: {
                second: '%Y-%m-%d<br/>%H:%M:%S',
                minute: '%Y-%m-%d<br/>%H:%M',
                hour: '%Y-%m-%d<br/>%H:%M',
                day: '%Y<br/>%m-%d',
                week: '%Y<br/>%m-%d',
                month: '%Y-%m',
                year: '%Y'
            }
        },

		series: [{
			name: lineName,
            type: chartType,
		 	data: []
		}]

   	});

   loadData(chartBox, collection, property);
   // return chartBox;
}

function loadMultiData(chart, listLine) {
    // var count = 0;
    // for (var i = 0; i < listLine.length; i++) {
    //     var res = requestForChart(listLine[i].collection, "total", listLine[i].property);
    //     res.success(function (data) {
    //         // console.log(data);
    //         var cont = JSON.parse(data);
    //         var point = [];
    //         for(j in cont) {
    //             point.push([Number(cont[j][0]), round2decimal(cont[j][1])]);
    //         }
    //         chart.addSeries({
    //             name: listLine[count].name,
    //             data: point
    //         }, true);
    //         count++;
    //     });
    // }

    // I must write three request separately because, the success of request will run randomly, so its hard to determine what exactly listline in success
    var resLine1 = requestForChart(listLine[0].collection, "total", listLine[0].property);
    resLine1.success(function (data) {
        // console.log(data);
        var cont = JSON.parse(data);
        var point = [];
        for(j in cont) {
            point.push([Number(cont[j][0]), round2decimal(cont[j][1])]);
        }
        chart.addSeries({
            name: listLine[0].name,
            data: point
        }, true);
    });

    var resLine2 = requestForChart(listLine[1].collection, "total", listLine[1].property);
    resLine2.success(function (data) {
        // console.log(data);
        var cont = JSON.parse(data);
        var point = [];
        for(j in cont) {
            point.push([Number(cont[j][0]), round2decimal(cont[j][1])]);
        }
        chart.addSeries({
            name: listLine[1].name,
            data: point
        }, true);
    });

    var resLine3 = requestForChart(listLine[2].collection, "total", listLine[2].property);
    resLine3.success(function (data) {
        // console.log(data);
        var cont = JSON.parse(data);
        var point = [];
        for(j in cont) {
            point.push([Number(cont[j][0]), round2decimal(cont[j][1])]);
        }
        chart.addSeries({
            name: listLine[2].name,
            data: point
        }, true);
    });

}

function drawMultiChart(idName, listLine, unit) {

    var multiChart = Highcharts.stockChart(idName, {

        chart: {
            height: 350,
            // type: 'spline',
            events: {
                load: function() {
                    var ser = this.series;
                    setInterval(function () {
                        var count = 0;
                        // for (var i = 0; i < listLine.length; i++) {
                        //     var res = requestForChart(listLine[i].collection, "short", listLine[i].property, ser[count].data.length);
                        //     res.success(function (data) {
                        //         // console.log(data);
                        //         if(data.length>0) {
                        //             var cont = JSON.parse(data);
                        //             for(j in cont) {
                        //                 ser[count].addPoint([Number(cont[j][0]), round2decimal(cont[j][1])], true, false);
                        //             }
                        //         } else {
                        //             // console.log(collection, ": no new data");
                        //         }
                        //         count++;
                        //     });
                        // }
                        var resLine1 = requestForChart(listLine[0].collection, "short", listLine[0].property, ser[0].data.length);
                        resLine1.success(function (data) {
                            // console.log(data);
                            if(data.length>0) {
                                var cont = JSON.parse(data);
                                for(j in cont) {
                                    ser[0].addPoint([Number(cont[j][0]), round2decimal(cont[j][1])], true, false);
                                }
                            } else {
                                // console.log(collection, ": no new data");
                            }
                            count++;
                        });

                        var resLine2 = requestForChart(listLine[1].collection, "short", listLine[1].property, ser[1].data.length);
                        resLine2.success(function (data) {
                            // console.log(data);
                            if(data.length>0) {
                                var cont = JSON.parse(data);
                                for(j in cont) {
                                    ser[1].addPoint([Number(cont[j][0]), round2decimal(cont[j][1])], true, false);
                                }
                            } else {
                                // console.log(collection, ": no new data");
                            }
                            count++;
                        });

                        var resLine3 = requestForChart(listLine[2].collection, "short", listLine[2].property, ser[2].data.length);
                        resLine3.success(function (data) {
                            // console.log(data);
                            if(data.length>0) {
                                var cont = JSON.parse(data);
                                for(j in cont) {
                                    ser[2].addPoint([Number(cont[j][0]), round2decimal(cont[j][1])], true, false);
                                }
                            } else {
                                // console.log(collection, ": no new data");
                            }
                            count++;
                        });

                    }, 2000);
                }
            }
        },
          
        rangeSelector: {
            buttons: [
                {
                    count: 1,
                    type: 'minute',
                    text: '1m'
                }, { 
                    count: 5,
                    type: 'minute',
                    text: '5m'
                }, {
                    count: 10,
                    type: 'minute',
                    text: '10m'
                }, {
                    type: 'all',
                    text: 'All'
                }
            ],
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
        xAxis: {
            type: 'datetime',
            dateTimeLabelFormats: {
                second: '%Y-%m-%d<br/>%H:%M:%S',
                minute: '%Y-%m-%d<br/>%H:%M',
                hour: '%Y-%m-%d<br/>%H:%M',
                day: '%Y<br/>%m-%d',
                week: '%Y<br/>%m-%d',
                month: '%Y-%m',
                year: '%Y'
            }
        },
        plotOptions: {
            line: {
                    dataLabels: {
                    enabled: true
                },
                enableMouseTracking: false
            }
        },
        // tooltip: {
        //     pointFormat: '<span style="color:{series.color}">{series.name}</span>: <b>{point.y}</b><br/>',
        //     split: false
        // },
        series: []
    });


    loadMultiData(multiChart, listLine);

    return multiChart;
}

// Display system time
function countTime(unix_epoch) {
    today = new Date(unix_epoch);
    var hh = today.getHours();
    var mm = today.getMinutes();
    var ss = today.getSeconds();
    $("#infor-systemTime").find(".infor-val").text(hh + "h" + ((mm<10)?"0":"") + mm + "'");
    setTimeout(function(){countTime(unix_epoch+(60-ss)*1000)}, (60-ss)*1000);
}

// print data in alert-box
function alertHelper(str) {
    var alertType, alertData, alertRes, alertStr;
    var responType = str.charAt(0);
    var responData = str.substr(2, str.length);
    var res;
    switch (responType) {
        case "S": { // success
            alertType = "alert-success";
            alertRes = "Thành công";
            res = 1;
        }break;
        case "W": { // warning
            alertType = "alert-warning";
            alertRes = "Cảnh báo";
            res = 0;
        } break;
        case "D": { // danger or fail
            alertType = "alert-danger";
            alertRes = "Thất bại";
            res = -1;
        } break;
        default: { // info
            alertType = "alert-info";
            alertRes = "Thông báo";
            res = 2;
        }
    }

    var random = Math.random().toString(36).substr(2, 5);
    alertStr = '<div id='+ random +' class="alert '+ alertType +'"><strong> '+ alertRes +'. </strong> <span>'+ responData +'</span><span class="close">×</span></div>';
   
    $("#field-alert").append(alertStr);

    $(".alert").each(function(e) {
        $(this).on("click", function() {
            $(this).remove();
        });
    });
    
    // automatic remove alert
    setTimeout(function(){
        $("#"+random+"").remove();
    }, 5000);

    return res;
}

function alertSystem(alertType, header, content) {
    alertStr = '<div class="alert '+ alertType +'"><strong> '+ header +'. </strong> <span>'+ content +'</span><span class="close">×</span></div>';
    $("#field-alert").append(alertStr);
    $(".alert").each(function(e) {
        $(this).on("click", function() {
            $(this).remove();
        });
    });
}

function requestStatus(coll, objName, type) {
    return $.post(
            "function/request.php",
            {
                request : "status",
                collection: coll,
                name : objName,
                key: type
            }
        )
}

function updateStatus(that, name, key) {

    var deviceStatus = requestStatus(setup, name, key);
    deviceStatus.success(function(respone) {
        var idx = respone.indexOf(":");
        var val = respone.substr(idx+1, respone.length);
        if(Number(val)) {
            $(that).addClass("switch-on");
        } else {
            $(that).removeClass("switch-on");
        }
    });

    setTimeout(function(){
        updateStatus(that, name, key);
    }, 5000);

}

function getDistinctArray(arr) {
    var nums = [];
    nums.push(arr[0]);
    for (var i = 1; i < arr.length; i++) {
        for (var j = 0; j < nums.length; j++) {
            if(arr[i]==nums[j])
                break;
        }
        if(j==(nums.length))
            nums.push(arr[i]);
    }
    return nums;
}

function round2decimal(num) {
    return +(Math.round(num + "e+2")  + "e-2");
}

});


