<?php

use yii\db\Migration;

class m160530_085212_routes_order extends Migration
{
    public function up()
    {
        $this->addColumn('routes','order','integer not null default \'0\'');
    }

    public function down()
    {
        $this->dropColumn('routes','order');
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
