<?php

namespace app\models;

class User extends \yii\db\ActiveRecord  implements \yii\web\IdentityInterface
{
    private static $user_;

    public static function tableName()
    {
        return 'users';
    }

    public function rules()
    {
        return  [
                    [['username','password','name'], 'required'],
                    ['password', 'hashPassword']
                ];
    }

    public function getId()
    {
        return $this->primaryKey;
    }

    static public function current()
    {
        return self::$user_;
    }

    public function hashPassword($attribute, $params)
    {
        $this->$attribute = \Yii::$app->getSecurity()->generatePasswordHash($this->$attribute);
    }

    public static function findIdentityByAccessToken($token, $type = null)
    {
          throw new NotSupportedException('"findIdentityByAccessToken" is not implemented.');
    }

    public static function findIdentity($id)
    {
        self::$user_ = static::findOne($id);
        return self::$user_;
    }

    public static function  findByUsername($username)
    {
        return static::findOne(['username'=>$username]);
    }

    public function getAuthKey()
    {
        return $this->auth_key;
    }

    public function validateAuthKey($authKey)
    {
        return $this->getAuthKey() === $authKey;
    }

    public function fields()
    {
        $fields = parent::fields();
        unset($fields['password']);
        return $fields;
    }

}