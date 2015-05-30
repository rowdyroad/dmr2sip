<?php

use yii\db\Schema;
use yii\db\Migration;

class m150523_212255_events extends Migration
{
    public function up()
    {
        $this->createTable('events', [
          'event_id' => 'pk',
          'time' => 'timestamp DEFAULT CURRENT_TIMESTAMP',
          'route_id' => 'integer NOT NULL',
          'source_number'=> 'string(200) NOT NULL'
        ]);
    }

    public function down()
    {
        echo "m150523_212255_events cannot be reverted.\n";

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
