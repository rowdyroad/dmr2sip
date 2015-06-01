<?php

use yii\db\Schema;
use yii\db\Migration;

class m150601_015133_point_name extends Migration
{
    public function up()
    {
	$this->addColumn('points','name','string(200)');
    }

    public function down()
    {
        echo "m150601_015133_point_name cannot be reverted.\n";

        return false;
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
