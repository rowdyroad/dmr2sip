<?php

namespace app\models;

class Route extends \yii\db\ActiveRecord
{
	static public function tableName()
	{
		return 'routes';
	}

 	public function rules()
    {
    	return 	[
    				[['source_point_id','destination_point_id'], 'required']
    			];
    }

    public function getSource_point()
    {
    	return $this->hasOne(Point::className(), ['point_id'=>'source_point_id']);
    }

    public function getDestination_point()
    {
    	return $this->hasOne(Point::className(), ['point_id'=>'destination_point_id']);
    }

    public function fields()
    {
    	return array_merge(parent::fields(),['source_point','destination_point']);
    }
}
