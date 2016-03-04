<?php
namespace app\controllers\configuration;
use Yii;
class NetworkController extends \yii\rest\Controller
{
    const NETWORK_INTERFACE = 'eth0';
    private function getConfig()
    {
        exec("ifconfig", $ifconfig);
        foreach ($ifconfig as $i=>$cfg) {
            if (strpos($cfg, self::NETWORK_INTERFACE) !== false) {
                preg_match("/inet addr:(?P<ip>[0-9\.]+).*Mask:(?P<mask>[0-9\.]+)/", $ifconfig[$i+1], $regs);
                break;
            } 
        }

        $interface_config = file_get_contents('/etc/network/interfaces.d/'.self::NETWORK_INTERFACE);
        if (preg_match(sprintf("/iface %s inet (dhcp|static)/", self::NETWORK_INTERFACE), $interface_config, $iregs)) {
            $type = $iregs[1];
        }
        if (preg_match(sprintf("/address ([0-9\.]+)/", self::NETWORK_INTERFACE), $interface_config, $iregs)) {
            $regs["ip"] = $iregs[1];
        }
        if (preg_match(sprintf("/netmask ([0-9\.]+)/", self::NETWORK_INTERFACE), $interface_config, $iregs)) {
            $regs["mask"] = $iregs[1];
        }
        if (preg_match(sprintf("/gateway ([0-9\.]+)/", self::NETWORK_INTERFACE), $interface_config, $iregs)) {
            $regs["gateway"] = $iregs[1];
        }
        if (preg_match(sprintf("/dns-nameservers (([0-9\.]+)\s+)+/", self::NETWORK_INTERFACE), $interface_config, $iregs)) {
            $dns = array_slice($iregs, 1,2);
        }
        return ['type'=>$type, "ip"=>$regs["ip"], "mask"=>$regs["mask"], "gateway"=>$regs["gateway"], "dns_primary"=>@$dns[0], "dns_secondary"=>@$dns[1]];;
    }

    public function actionIndex()
    {
        return $this->getConfig();
    }

    public function actionUpdate()
    {
        $data = \Yii::$app->request->bodyParams;

        if ($data['type'] == 'dhcp') {
            file_put_contents('/etc/network/interfaces.d/'.self::NETWORK_INTERFACE, sprintf("auto %s\niface %s inet dhcp\n", 
                                                                        self::NETWORK_INTERFACE,
                                                                        self::NETWORK_INTERFACE));
        } elseif ($data['type'] == 'static') {
            $dns = @$data['dns_primary']." ".@$data['dns_secondary'];
            file_put_contents('/etc/network/interfaces.d/'.self::NETWORK_INTERFACE, sprintf("auto %s\niface %s inet static\naddress %s\nnetmask %s\n",
                                                                        self::NETWORK_INTERFACE,
                                                                        self::NETWORK_INTERFACE,
                                                                        $data['ip'],
                                                                        $data['mask']
            ));

            if ($dns != " ") {
                file_put_contents('/etc/network/interfaces.d/'.self::NETWORK_INTERFACE, sprintf("dns-nameservers %s\n", $dns), FILE_APPEND);
            }

            if (!empty($data['gateway'])) {
                file_put_contents('/etc/network/interfaces.d/'.self::NETWORK_INTERFACE, sprintf("gateway %s\n", $data['gateway']), FILE_APPEND);
            }
        }
        system(sprintf("ifdown %s && ifup %s", self::NETWORK_INTERFACE, self::NETWORK_INTERFACE));
    }
}