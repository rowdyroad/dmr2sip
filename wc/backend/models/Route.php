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
                    [['source_point_id','source_number', 'destination_point_id', 'destination_number', 'phone_mode'], 'required'],
                    ['destination_number', 'filter', 'filter'=>'json_encode']
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

    public function toArray( $fields = [], $expand = [], $recursive = true )
    {
       $arr = parent::toArray($fields, $expand, $recursive);
       $arr['destination_number'] = json_decode($arr['destination_number'],true);
       return $arr;
    }
}
