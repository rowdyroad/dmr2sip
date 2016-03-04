<?php
namespace app\controllers\configuration;
use Yii;
class NetworkController extends \yii\rest\Controller
{
    const NETWORK_INTERFACE = 'eth0';
    public function actionIndex()
    {
        exec("ifconfig", $ifconfig);
        foreach ($ifconfig as $i=>$cfg) {
            if (strpos($cfg, self::NETWORK_INTERFACE) !== false) {
                preg_match("/inet addr:(?P<ip>[0-9\.]+).*Mask:(?P<mask>[0-9\.]+)/", $ifconfig[$i+1], $regs);
		break;
            } 
        }
        return ['type'=>'dhcp', "ip"=>$regs["ip"], "mask"=>$regs["mask"]];
    }

    public function actionUpdate()
    {

    }
}