<?php
namespace app\components;
use Yii;

class Service extends \yii\base\Component
{
	var $service;
	public function reload()
	{
		$command = sprintf('sudo /etc/init.d/%s reload 2>&1 > /dev/null', $this->service);
		system($command);
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