<?php
use yii\grid\GridView;
use yii\widgets\ActiveForm;

use yii\helpers\Html;

if (isset($model)) {
	$form = ActiveForm::begin();
	echo $form->field($model, 'name');
	echo $form->field($model, 'code');
	echo $form->field($model, 'mammal_group_id')->dropDownList($mammals_groups?:[]);
	echo Html::submitButton('Submit');
	ActiveForm::end();
}

echo GridView::widget([
    'dataProvider' => $dataProvider
]);