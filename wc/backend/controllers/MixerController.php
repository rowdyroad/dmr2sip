<?php
namespace app\controllers;
use yii\web\Controller;
use Yii;



class MixerController extends Controller
{
    private function getCards()
    {
         if ($data = \Yii::$app->docker->exec('dmr2sip-service', 'cat /proc/asound/cards')) {
            if (preg_match_all('/(\d+)\s+\[([^\]]+)\]/im', $data, $regs,PREG_SET_ORDER)) {
                $ret = [];
                foreach ($regs as $reg) {
                    $ret[] = (object)["device_id"=>intval($reg[1]) + 1, "name"=>trim($reg[2])];
                }
                return $ret;
            }
        }

        return false;
    }

    private function getCard($device_id)
    {
        if ($cards = $this->getCards()) {
            foreach ($cards as $card) {
                if ($card->device_id == $device_id) {
                    return $card;
                    break;
                }
            }
        }
        return false;
    }
    public function actionIndex()
    {
        if ($cards = $this->getCards()) {
            return $cards;
        }
        return [];
    }

    public function actionUpdate($device_id)
    {
        if (($ret = $this->getCard($device_id)) === false) {
            throw new \yii\web\HttpException(404, "Sound device not found");
        }

        if ($data = Yii::$app->getRequest()->getBodyParams()) {
            foreach ($data['controls'] as $control) {
                foreach ($control['controls'] as $c) {
                    if ($c['type'] == 'percent' && $c['name'] == 'volume') {
                        \Yii::$app->docker->exec('dmr2sip-service',
                                sprintf('/usr/bin/amixer -c %d sset \'%s\' %d%%', $ret->device_id - 1, $control['name'], $c['value']));
                    }
                    if ($c['type'] == 'enum') {
                        echo \Yii::$app->docker->exec('dmr2sip-service',
                                sprintf('/usr/bin/amixer -c %d sset \'%s\' \'%s\'', $ret->device_id - 1, $control['name'], $c['value']));
                    }

                    if ($c['type'] == 'switch') {
                        if ($c['name'] == 'mute') {
                            \Yii::$app->docker->exec('dmr2sip-service',
                                    sprintf('/usr/bin/amixer -c %d sset \'%s\' %s', $ret->device_id - 1, $control['name'], $c['value'] ? 'mute' : 'unmute'));
                        }
                        if ($c['name'] == 'capture') {
                            \Yii::$app->docker->exec('dmr2sip-service',
                                    sprintf('/usr/bin/amixer -c %d sset \'%s\' %s', $ret->device_id - 1, $control['name'], $c['value'] ? 'cap' : 'nocap'));
                        }
                    }
                }
            }
        }
        $this->actionView($device_id);
    }

    public function actionView($device_id)
    {
        if (($ret = $this->getCard($device_id)) === false) {
            throw new \yii\web\HttpException(404, "Sound device not found");
        }

        if ($data = \Yii::$app->docker->exec('dmr2sip-service', sprintf('/usr/bin/amixer -c %d', $ret->device_id - 1))) {
            $items = explode("Simple mixer control ", $data);
            $elements = [];

            foreach ($items as $item) {

                $elem = new \stdClass;
                $elem->controls = [];

                if (preg_match('/\'([^\']+)\'/im', $item, $regs)) {
                    $elem->name = $regs[1];
                } else {
                    continue;
                }
                if (preg_match('/Capabilities: (.+)$/im', $item, $regs)) {
                    $item_types = explode(" ", $regs[1]);
                } else {
                    continue;
                }

                $types = ['p' => 'playback', 'c' => 'capture'];

                foreach ($item_types as $type) {
                    $control = new \stdClass;
                    $prefix = @$types[$type[0]];
                    $control->type = $type;
                    switch ($type) {
                        case 'enum':
                            if (preg_match(sprintf('/Items: \'(.+)\'$/im', $prefix), $item, $regs)) {
                                $control->variants = explode("' '", $regs[1]);
                            }
                            if (preg_match(sprintf('/Item\d+: \'(.+)\'$/im', $prefix),$item, $regs)) {
                                $control->value = $regs[1];
                            }
                            $elem->controls[] = $control;
                        break;
                        case 'cvolume':
                        case 'pvolume':
                            $control->type = "percent";
                            $control->name = "volume";
                            if (preg_match(sprintf('/Limits: %s (\d+) - (\d+)$/im', $prefix), $item, $regs)) {
                                $min = intval($regs[1]);
                                $max = intval($regs[2]);
                            }

                            if (preg_match(sprintf('/%s channels: (.+)$/im', $prefix), $item, $regs)) {
                                $channels = explode(" - ", $regs[1]);
                                $val = 0;

                                foreach ($channels as $channel) {
                                    if (preg_match(sprintf('/%s: %s \d+ \[(\d+)%%\]/im', $channel, $prefix), $item, $regs1)) {
                                        $control->value = intval($regs1[1]);
                                        break;
                                    }
                                }
                                $elem->controls[] = $control;
                            }
                        break;
                        case 'pswitch':
                            $control->type = "switch";
                            $control->name = "mute";
                            if (preg_match(sprintf('/%s channels: (.+)$/im', $prefix), $item, $regs)) {
                                $channels = explode(" - ", $regs[1]);
                                foreach ($channels as $channel) {
                                    if (preg_match(sprintf('/%s:.+Playback.+\[(on|off)\]/im', $channel), $item, $regs1)) {
                                        $control->value = $regs1[1] !== 'on';
                                        break;
                                    }
                                }
                                $elem->controls[] = $control;
                            }
                        break;
                        case 'cswitch':
                            $control->type = "switch";
                            $control->name = "capture";
                            if (preg_match(sprintf('/%s channels: (.+)$/im', $prefix), $item, $regs)) {
                                $channels = explode(" - ", $regs[1]);
                                foreach ($channels as $channel) {
                                    if (preg_match(sprintf('/%s:.+Capture.+\[(on|off)\]/im', $channel, $prefix), $item, $regs1)) {
                                        $control->value = $regs[1] !== 'on';
                                        break;
                                    }
                                }
                                $elem->controls[] = $control;
                            }
                        break;
                    }
                }
                $elements[] = $elem;
            }
            $ret->controls = $elements;
            return $ret;
        }
    }
}