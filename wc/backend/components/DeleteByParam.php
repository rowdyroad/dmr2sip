<?php
namespace app\components;

use yii\behaviors\AttributeBehavior;
use yii\db\ActiveRecord;
use Yii;

trait DeleteByParam
{
    public static function deleteAll($condition = '', $params = [])
    {
        $command = static::getDb()->createCommand();
        $command->update(static::tableName(), [
                                                'is_deleted' => true,
                                                'deleted_user_id'=>\Yii::$app->user->id,
                                                'deleted_username'=>\app\models\User::current()->username,
                                                'deleted_time' => new \yii\db\Expression("NOW()")
                                                ], $condition, $params);
        return $command->execute();
    }

    static public function find()
    {
        return parent::find()->andWhere(['is_deleted'=>false]);
    }
}
