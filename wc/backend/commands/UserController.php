<?php
namespace app\commands;
use yii\console\Controller;
use app\models\User;
use Yii;

class UserController extends Controller
{
    public function actionIndex($username, $password, $name, $is_admin)
    {
                $r = new User;
                $r->username = $username;
                $r->password = $password;
                $r->is_admin = $is_admin;
                $r->name = $name;
                $r->save();
    }

    public function actionPasswordReset($username)
    {
        if ($user = User::findOne(['username'=>$username])) {
            $user->password = '123123';
            $user->save();
        }
    }
}
