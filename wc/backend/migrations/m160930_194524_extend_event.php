<?php

use yii\db\Migration;

class m160930_194524_extend_event extends Migration
{
    public function up()
    {
         $this->addColumn('events','source','text');
         \Yii::$app->db->createCommand('UPDATE events SET source = CONCAT(\'{"number": "\', source_number, \'"}\')')->execute();

         $this->addColumn('events','destination','text');
         $this->addColumn('events','result', 'string');
         $this->dropColumn('events','source_number');
         $this->addColumn('events', 'source_point_id', 'integer');
         $this->alterColumn('events', 'route_id', 'integer');
    }

    public function down()
    {
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
