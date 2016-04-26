<?php

use yii\db\Migration;

class m160426_145223_change_destination_number extends Migration
{
    public function up()
    {
        $this->alterColumn('routes', 'destination_number', 'text');
    }

    public function down()
    {
        echo "m160426_145223_change_destination_number cannot be reverted.\n";

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
