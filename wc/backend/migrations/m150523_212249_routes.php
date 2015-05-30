<?php

use yii\db\Schema;
use yii\db\Migration;

class m150523_212249_routes extends Migration
{
    public function up()
    {
        $this->createTable('routes', [
          'route_id' => 'pk',
          'source_point_id' => 'integer NOT NULL',
          'source_number' => 'string(200) NOT NULL',
          'destination_point_id' => 'integer NOT NULL',
          'destination_number' => 'string(200) NOT NULL'
        ]);
        $this->createIndex('routes_unique','routes',['source_point_id','destination_point_id'],true);

    }

    public function down()
    {
        echo "m150523_212249_routes cannot be reverted.\n";

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
