<?php
namespace app\components;
use Yii;

class Service extends \yii\base\Component
{
    public $service;

    public function restart()
    {
       return \Yii::$app->docker->restart($this->service);
    }

    public function isRunning()
    {

        if ($status = \Yii::$app->docker->status($this->service)) {
            return @$status->State->Running;
        }
        return false;
    }

    public function log()
    {
        return \Yii::$app->docker->log($this->service);
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
