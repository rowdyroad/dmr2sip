<?php
namespace app\components;
use Yii;

class Service extends \yii\base\Component
{
    var $service;
    private $state_ = null;
    private function execute($method, $waitfor = false)
    {
        $command = sprintf('sudo /etc/init.d/%s %s', $this->service, $method);
        $ret = exec($command);
        if ($waitfor) {
            return stripos(exec($command), $waitfor) !== false;
        }
        return true;
    }

    public function restart()
    {
        `sudo tools/service/restart`;
        return true;
    }

    public function isRunning()
    {
        if ($status = @json_decode(`sudo tools/service/status`)) {
            return @$status->State->Running;
        }
        return false;
    }

    public function log()
    {
        return `sudo tools/service/log`;
    }

    public function init()
    {
        \yii\base\Event::on(\yii\db\ActiveRecord::className(),
                            \yii\db\ActiveRecord::EVENT_AFTER_DELETE, [$this, 'restart']);
        \yii\base\Event::on(\yii\db\ActiveRecord::className(),
                            \yii\db\ActiveRecord::EVENT_AFTER_INSERT, [$this, 'restart']);
        \yii\base\Event::on(\yii\db\ActiveRecord::className(),
                            \yii\db\ActiveRecord::EVENT_AFTER_UPDATE, [$this, 'restart']);
    }
}
