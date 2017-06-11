<!-- 00h44' May 16 2017 -->
<?php 

include 'function/communicate.php';
include 'function/layout.php';

$cmdResult = sendCommandToMaster(" ");
// $cmdResult = true;

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no" />
    <title>Grow Box</title>
    <!-- <script src="js/highcharts.js"></script> -->
    <script src="scripts/js/highstock.js"></script>
    <script src="scripts/js/modules/exporting.js"></script>
    <script src="scripts/js/themes/grid-light.js"></script>

    <!-- JS -->
    <script type="text/javascript" src="scripts/jquery/jquery.js"></script>
    <script type="text/javascript" src="scripts/jquery/jquery-ui.min.js"></script>
    <script type="text/javascript" src="scripts/jquery.ui.touch-punch.min.js"></script>
    <script type="text/javascript" src="scripts/script.js"></script>

    <!-- CSS -->
    <link rel="stylesheet" href="https://fonts.googleapis.com/css?family=Open+Sans">
    <link rel="stylesheet" type="text/css" href="fonts/font-awesome/css/font-awesome.min.css">

    <!-- Bootstrap -->
    <link rel="stylesheet" type="text/css" href="css/bootstrap.min.css">
    <link rel="stylesheet" type="text/css" href="css/jquery-ui.min.css">
    <link rel="stylesheet" type="text/css" href="css/highcharts.css">
    <link rel="stylesheet" type="text/css" href="css/index.css">
    <!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/html5shiv/3.7.3/html5shiv.min.js"></script>
      <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
    <![endif]-->
</head>
<body class="plant">
    <nav>
        <div class="container">
            <div class="nav">
                <h1>Environment</h1>
                <div class="option">
                    <button type="button" class="btn adv-option btn-primary">Advanced options</button>
                </div>
            </div>
        </div>
    </nav>
<?php 
    printSectionInfor($conn);
?>
    <section class="sec-graph">
        <div class="container">
            <div class="row">
                <div class="col-lg-12 adv-option-content">
                    <div class="graph">
                        <div class="graph-setting">
                            <h3>Biểu đồ tiến trình thiết lập nhiệt độ</h3>
                            <div class="box-setting">
                                <div class="elem-content request-area" object="temptAndHumy" type="temptInBox">
                                    <button type="button" name="button" <?php echo 'class="submit butn sendData '.($cmdResult ? "enable ":"").'"'; ?> >Đặt nhiệt độ</button>
                                    <div class="box-setting-val">
                                        <input type="number" class="input-number" reference="setTempt" min="15" max="40" value=<?php echo getField($conn, collSetup, "title", "temptAndHumy", "temptInBox"); ?> >
                                        <label for="setting-temptInBox">°C</label>
                                    </div> 
                                </div>
                            </div>
                        </div>
                        <div id="graphTemptProgress"></div>
                    </div> 
                </div>
                <div class="col-lg-12 adv-option-content">
                    <div class="graph">
                        <div class="graph-setting">
                            <h3>Biểu đồ tiến trình thiết lập độ ẩm</h3>
                            <div class="box-setting">
                                <div class="elem-content request-area" object="temptAndHumy" type="humidity">
                                    <button type="button" name="button" <?php echo 'class="submit butn sendData '.($cmdResult ? "enable ":"").'"'; ?>>Đặt độ ẩm</button>
                                    <div class="box-setting-val">
                                        <input type="number" class="input-number" reference="setHumy" min="50" max="100" value=<?php echo getField($conn, collSetup, "title", "temptAndHumy", "humidity"); ?> >
                                        <label>%</label>
                                     </div>
                                </div>
                            </div>
                        </div>
                        <div id="graphHumyProgress"></div>
                    </div> 
                </div>
<!--                 <div class="col-lg-12 adv-option-content">
                    <div class="graph">
                        <div class="graph-setting">
                            <h3>Biểu đồ đèn thắp sáng</h3>
                        </div>
                        <div id="graphLamp"></div>
                    </div> 
                </div> -->
            </div>
            <div class="row">
                <div class="col-lg-6">
                    <div class="graph">
                        <div class="graph-setting">
                            <h3>Biểu đồ nhiệt độ</h3>
                            <div class="box-setting">
                                <div class="elem-content request-area elem-control-symbol" object="temptAndHumy" type="temptInBox">
                                    <span <?php echo 'class="butn-symbol enable-prop '.((getField($conn, collSetup, "title", "temptAndHumy", "enable_tempt")==true)?"enable":"").'"'; ?> type="enable_tempt"><i class="fa fa-check" aria-hidden="true"></i></span>
                                    <button type="button" name="button" <?php echo 'class="submit butn sendData '.($cmdResult ? "enable ":"").'"'; ?>>Đặt nhiệt độ</button>
                                    <div class="box-setting-val">
                                        <input type="number" class="input-number" reference="setTempt" min="15" max="40" value=<?php echo getField($conn, collSetup, "title", "temptAndHumy", "temptInBox"); ?> >
                                        <label>°C</label>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <div id="graphTemptInBox"></div>
                    </div>
                </div>
                <div class="col-lg-6">
                    <div class="graph">
                        <div class="graph-setting">
                            <h3>Biểu đồ độ ẩm</h3>
                            <div class="box-setting">
                                <div class="elem-content request-area" object="temptAndHumy" type="humidity">
                                    <span <?php echo 'class="butn-symbol enable-prop '.((getField($conn, collSetup, "title", "temptAndHumy", "enable_humy")==true)?"enable":"").'"'; ?> type="enable_humy"><i class="fa fa-check" aria-hidden="true"></i></span>
                                    <button type="button" name="button" <?php echo 'class="submit butn sendData '.($cmdResult ? "enable ":"").'"'; ?>>Đặt độ ẩm</button>
                                    <div class="box-setting-val">
                                        <input type="number" class="input-number" reference="setHumy" min="50" max="100" value=<?php echo getField($conn, collSetup, "title", "temptAndHumy", "humidity"); ?> >
                                        <label>%</label>
                                     </div>
                                </div>
                            </div>
                        </div>
                        <div id="graphHumyInBox"></div>
                    </div> 
                </div>
                <div class="clearfix"></div>
            </div>
        </div>
    </section>
    <div class="field-alert" id="field-alert">
        <?php 
            if(!$cmdResult)
                echo    '<div class="alert alert-danger" id="system-error" onclick="myFunction()">
                            <strong>Không thể kết nối tới trung tâm điều khiển.</strong> 
                            </br> 
                            <span>Yêu cầu kiểm tra log và khởi động lại hệ thống</span> 
                            <span class="close">×</span>
                        </div>';

                echo    '<script>
                        function myFunction() {
                            var elem = document.getElementById("system-error");
                            elem.parentNode.removeChild(elem);
                        }
                        </script>';
        ?>   
    </div>
<?php 

    printSectionSetting($conn, $cmdResult);

?> 
    

</body>
</html>