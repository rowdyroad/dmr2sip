<?php

use yii\db\Migration;

class m160214_115855_point_config extends Migration
{
    public function up()
    {
	$ret = [];
	if ($points = \app\models\Point::find()->all()) {
	    foreach ($points as $point) {
		switch ($point->type) {
		    case 'dmr':
			list($address, $port) = explode(":", $point->id);
			list($auth_key, $delta) = explode(":", $point->password);
		
			$ret[$point->point_id] = json_encode([
			    'address'=>$address,
			    'port'=>$port,
			    'auth_key'=>$auth_key,
			    'delta'=>$delta
			]);
		    break;
		    case 'sip':
			list($device, $id) = explode("/", $point->id);
			$ret[$point->point_id] = json_encode([ 
			    'device'=> $device,
			    'id'=>$id,
			    'password'=>$point->password
			]);
			
		    break;
		}
	    }
	}
	
	$this->dropColumn('points','id');
	$this->dropColumn('points','password');
	$this->addColumn('points','configuration','text');

	foreach ($ret as $point_id=>$configuration) {
	    \app\models\Point::updateAll(['configuration'=>$configuration], ['point_id'=>$point_id]);
	}
    }

    public function down()
    {
	$this->dropColumn('points','configuration');
	$this->addColumn('points','id','string');
	$this->addColumn('points','password','string');
    }

    /*
    // Use safeUp/safeDown to run migration code within a transaction
    public function safeUp()
    {
    }

    public function safeDown()
    {
    }
    */
}
