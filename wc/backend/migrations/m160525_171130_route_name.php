<?php

use yii\db\Migration;

class m160525_171130_route_name extends Migration
{
    public function up()
    {  
        $this->addColumn('routes','name','string');
    }

    public function down()
    {
        $this->dropColumn('routes','name');
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
