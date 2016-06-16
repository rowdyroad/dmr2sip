<?php

use yii\db\Schema;
use yii\db\Migration;

class m150602_124041_route_index extends Migration
{
    public function up()
    {
	$this->createIndex('routes_unique', 'routes',['source_point_id','source_number'], true);
    }

    public function down()
    {
        echo "m150602_124041_route_index cannot be reverted.\n";

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
