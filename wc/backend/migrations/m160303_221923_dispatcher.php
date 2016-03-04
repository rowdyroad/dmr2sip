<?php

use yii\db\Migration;

class m160303_221923_dispatcher extends Migration
{
    public function up()
    {
        $user = new \app\models\User;
        $user->name = 'Dispatcher';
        $user->username = 'dispatcher';
        $user->password = '123123';
        $user->save();
    }

    public function down()
    {
        echo "m160303_221923_dispatcher cannot be reverted.\n";

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
