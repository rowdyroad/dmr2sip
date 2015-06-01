<?php
namespace app\controllers;
use yii\rest\ActiveController;
use Yii;
class UserController extends ActiveController
{
    public $serializer = [
        'class' => 'yii\rest\Serializer',
        'collectionEnvelope' => 'data',
    ];
    public $modelClass = 'app\models\User';

    public function actionCheck()
    {
        if (Yii::$app->user->isGuest) { // item does not exist
            throw new \yii\web\HttpException(401, 'Access Denied');
        }
        return \app\models\User::current();
    }

    public function actionLogin()
    {
        $auth = (object)Yii::$app->getRequest()->getBodyParams();
        if (isset($auth->username) && isset($auth->password) && $user = \app\models\User::findByUsername($auth->username)) {
             if (Yii::$app->security->validatePassword($auth->password, $user->password)) {
                Yii::$app->user->login($user);
                return $user;
             }
        }


        \Yii::$app->response->statusCode = 422;
        return [['field'=>'username','message'=>'Blank or invalid username field'],['field'=>'password','message'=>'Blank or invalid password field']];
    }

    public function actionLogout()
    {
        if (Yii::$app->user->isGuest) {
            //throw new \yii\web\HttpException(401, 'Access Denied');
            return;
        }
        Yii::$app->user->logout();
    }
}