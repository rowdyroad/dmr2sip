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
        return  [
                    [['name','source_point_id','source_number', 'destination_point_id', 'destination_number','order'], 'required'],
                    [['source_number','destination_number'], 'filter', 'filter'=>'json_encode'],
                    ['phone_mode', 'safe']
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

    public function toArray(array $fields = [],array $expand = [], $recursive = true )
    {
       $arr = parent::toArray($fields, $expand, $recursive);
       $arr['destination_number'] = json_decode($arr['destination_number'],true);
       $arr['source_number'] = json_decode($arr['source_number'],true);
       return $arr;
    }
}
