<?php

namespace app\models;

class Point extends \yii\db\ActiveRecord
{
	static public function tableName()
	{
		return 'points';
	}

 	public function rules()
    {
    	return 	[
    				[['type','name','id'], 'required'],
                    ['password','safe']
    			];
    }
}
