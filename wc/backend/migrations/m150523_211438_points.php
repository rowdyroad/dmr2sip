<?php

use yii\db\Schema;
use yii\db\Migration;

class m150523_211438_points extends Migration
{
    public function up()
    {
        $this->createTable('points', [
          'point_id' => 'pk',
          'type' => 'string(64) NOT NULL',
          'id' => 'string(200) NOT NULL',
          'password' => 'string(200) NOT NULL',
          'status' => 'integer NOT NULL DEFAULT \'0\''
        ]);
    }

    public function down()
    {
        echo "m150523_211438_points cannot be reverted.\n";

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
