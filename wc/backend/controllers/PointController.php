<?php
namespace app\controllers;
use yii\rest\ActiveController;
use Yii;
class PointController extends ActiveController
{
	public $serializer = [
        'class' => 'yii\rest\Serializer',
        'collectionEnvelope' => 'data',
    ];
	public $modelClass = 'app\models\Point';

	public function actions()
	{
		return array_diff_key(parent::actions(), ['index'=>null]);
	}

	public function actionIndex()
	{
		$model = $this->modelClass;
        return  $model::find()->all();
	}
}