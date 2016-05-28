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
    public function reload()
    {
        return $this->execute("reload","done");
    }

    public function restart()
    {
        return $this->execute("restart", "done");
    }

    public function isRunning()
    {
        if ($this->state_ == null) {
            $this->state_ = $this->execute("status", "is running");
        }
        return $this->state_;
    }

    public function log()
    {
        return file_get_contents("/opt/dmr/log/commutator.log");
    }

    public function init()
    {
        \yii\base\Event::on(\yii\db\ActiveRecord::className(),
                            \yii\db\ActiveRecord::EVENT_AFTER_DELETE, [$this, 'reload']);
        \yii\base\Event::on(\yii\db\ActiveRecord::className(),
                            \yii\db\ActiveRecord::EVENT_AFTER_INSERT, [$this, 'reload']);
        \yii\base\Event::on(\yii\db\ActiveRecord::className(),
                            \yii\db\ActiveRecord::EVENT_AFTER_UPDATE, [$this, 'reload']);
    }
}