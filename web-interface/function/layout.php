<?php 

include 'database.php';

function console_log($data) {
    echo '<script>';
    echo 'console.log('. json_encode( $data ) .')';
    echo '</script>';
}

function second2hour($ss) {
    return $ss/3600;
}

function second2minute($ss) {
    return $ss/60;
}

function printSectionSetting($conn, $cmdResult) {

    $lamp = getDocument($conn, collSetup, "title", "lamp");
    $temptAndHumy = getDocument($conn, collSetup, "title", "temptAndHumy");
    $vent = getDocument($conn, collSetup, "title", "ventilation");

    $scheTemptAndHumy = array_map("second2hour", iterator_to_array($temptAndHumy->schedule, true));

    echo '<section class="sec-setting">
        <div class="container">
            <div class="row">
                <div class="col-sm-6 col-md-6 col-lg-4">
                   <div class="set-element box-shadow">
                        <div class="switch-device request-area" object="lamp" type="state">
                            <span class="elem-name switch-name">Đèn chiếu sáng</span>
                            <div class="switch-button sw-type-state '.((!($lamp->enable))&($cmdResult) ? "enable ":"").' switchData '.( $lamp->state ? "switch-on":"").'"></div>
                        </div>
                        <div class="elem-enable request-area" object="lamp" type="enable">
                            <span class="switch-name">Chế độ tự động</span>
                            <div class="switch-button sw-type-enable switchData '.($cmdResult ? "enable ":"").(($lamp->enable) ? "switch-on":"").'"></div>
                        </div>
                        <div class="elem-content request-area" object="lamp" type="schedule">
                            <label for="lightOnFrom">Bật đèn</label>
                            <input type="number" id="lightOnFrom" name="lightOnFrom" class="input-number" min="0" max="24" value="'.(second2hour( ($lamp->schedule[0]) % 86400 )).'">
                            <label for="lightOnTo">tới</label>
                            <input type="number" id="lightOnTo" name="lightOnTo" class="input-number" min="0" max="24" value="'.(second2hour( $lamp->schedule[1] )).'">
                            <em>h</em>
                            <button type="button" name="button" class="submit butn sendData '.($cmdResult ? "enable ":"").'">Thiết lập</button>
                        </div>
                   </div>
                </div>
                <div class="col-sm-6 col-md-6 col-lg-4">
                    <div class="set-element box-shadow">
                        <div class="switch-device request-area" object="temptAndHumy" type="reading">
                            <span class="elem-name">Đo nhiệt độ, độ ẩm</span>
                            <button type="button" class="btn hand-reading '.($cmdResult ? "enable ":"").' btn-primary">ĐỌC</button>
                        </div>
                        <div class="elem-enable request-area" object="temptAndHumy" type="enable">
                            <span class="switch-name">Chế độ tự động</span>
                            <div class="switch-button sw-type-enable switchData '.($cmdResult ? "enable ":"").(($temptAndHumy->enable) ? "switch-on":"").'"></div>
                        </div>
                        <div class="elem-content request-area" object="temptAndHumy" type="schedule">
                            <label for="tempAndHumySchedule">Thời gian</label>
                            <input type="text" id="tempAndHumySchedule" name="tempAndHumySchedule" class="input-text" value="'.implode(",", $scheTemptAndHumy).'" placeholder="1,5,9,13,17,21">
                            <button type="button" name="button" class="submit butn sendData '.($cmdResult ? "enable ":"").'">Thiết lập</button>
                        </div>
                    </div>
                </div>
                <div class="col-sm-6 col-md-6 col-lg-4">
                    <div class="set-element box-shadow">
                        <div class="switch-device request-area" object="ventilation" type="state">
                            <span class="elem-name switch-name">Cửa thông hơi</span>
                            <div class="switch-button sw-type-state '.((!($vent->enable))&($cmdResult) ? "enable ":"").' switchData '.(($vent->state) ? "switch-on":"").'"></div>
                        </div>
                        <div class="elem-enable request-area" object="ventilation" type="enable">
                            <span class="switch-name">Chế độ tự động</span>
                            <div class="switch-button sw-type-enable switchData '.($cmdResult ? "enable ":"").(($vent->enable) ? "switch-on":"").'"></div>
                        </div>
                        <div class="elem-content request-area" object="ventilation" type="cycle">
                            <label for="venInvervalOpen">Mở</label>
                            <input type="number" id="venInvervalOpen" name="venInvervalOpen" class="input-number big-size" min="0" max="4294967296" value="'.(second2minute($vent->cycle[0])).'" placeholder="2">
                            <label for="venInvervalInterval">đóng</label>
                            <input type="number" id="venInvervalInterval" name="venInvervalInterval" class="input-number big-size" min="0" max="4294967296" value="'.(second2minute($vent->cycle[1])).'" placeholder="75">
                            <em>m</em>
                            <button type="button" name="button" class="submit butn sendData '.($cmdResult ? "enable ":"").'">Thiết lập</button>
                        </div>
                    </div>
                </div>
            <div class="clearfix"></div>
            </div>
        </div>
    </section>';
}

function objectInfor($name, $value, $unit) {
    echo '<div class="col-xs-6 col-sm-6 col-md-3 border-right">
            <div class="infor-content clearfix">
                <span class="infor-name">'.$name.'</span>
                <div>
                    <span class="infor-val">'.$value.'</span>
                    <span class="infor-unit"><em>'.$unit.'</em></span>
                </div>
            </div>
        </div>';
}
function printSectionInfor($conn) {

    echo '<section class="sec-infor-current">
        <div class="container">
            <div class="row">
                <div class="col-xs-12">
                    <div class="infor box-shadow clearfix">';

                    $tempt = getLastDocument($conn, collTempt);
                    $humy = getLastDocument($conn, collHumy);
                    $lamp = getLastDocument($conn, collLamp);

                    $tm = time();
                    $hh = date('G', $tm);
                    $mm = date('i', $tm);
                    $tm_str = $hh . "h" . $mm . "'";

                    objectInfor("Nhiệt độ", round($tempt->temptInBox), "°C");
                    objectInfor("Độ ẩm", round($humy->humidity), "%");
                    objectInfor("Ánh sáng", $lamp->lightLevel, "lux");

                    echo '<div class="col-xs-6 col-md-6 col-md-3 border-right">
                        <div class="infor-content clearfix">
                            <span class="infor-name">Thời gian</span>
                            <div id="infor-systemTime">
                                <span class="infor-val" epoch-time="'.$tm.'">'.$tm_str.'</span>
                                <span class="infor-unit"><em></em></span>
                            </div>
                        </div>
                    </div>';
                    echo '</div>
                </div>
            </div>
        </div>
    </section>';

}

?>