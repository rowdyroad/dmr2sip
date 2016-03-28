<?php

use yii\db\Migration;

class m160327_005656_phone_mode extends Migration
{
    public function up()
    {
        $this->addColumn('routes','phone_mode','boolean default \'0\'');
    }

    public function down()
    {
        $this->dropColumn('routes','phone_mode');
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
