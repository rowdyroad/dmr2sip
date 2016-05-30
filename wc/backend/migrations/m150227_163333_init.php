<?php

use yii\db\Schema;
use yii\db\Migration;

class m150227_163333_init extends Migration
{
    public function up()
    {
        $this->createTable('users', [
          'user_id' => 'pk',
          'username' => 'string(64) NOT NULL',
          'password' => 'string(100) NOT NULL',
          'name' => 'string(200) NOT NULL',
          'is_admin' => 'tinyint(4) NOT NULL DEFAULT \'0\''
        ]);

        $admin = new \app\models\User;
        $admin->username = 'admin';
        $admin->password = '123123';
        $admin->is_admin = 1;
        $admin->name = 'Administrator';
        $admin->save();

        $disp = new \app\models\User;
        $disp->username = 'disp';
        $disp->password = '123123';
        $disp->name = 'Dispatcher';
        $disp->save();
    }

    public function down()
    {
        echo "m150227_163333_init cannot be reverted.\n";

        return false;
    }
}
