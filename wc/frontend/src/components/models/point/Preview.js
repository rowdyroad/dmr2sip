import React from 'react'
import {Link} from 'react-router'

export default (props) => (
    <Link to={"/points/" + props.point_id} style={{textAlign:'center'}}>
    #{props.point_id}
        <div className={"point-item-status point-item-status-" + (props.status ? props.status : 0)}></div>
        <h1 className="point-item-name">{props.name}</h1>
        <h4 className="point-item-type">{props.type}</h4>
        {props.type === 'dmr' && props.configuration ? <h6 className="point-item-configuration">Sound Device: {props.configuration.device_index}</h6> : null}
        {props.type === 'sip' && props.configuration ? <h6 className="point-item-configuration">Caller ID: {props.configuration.username}({props.configuration.host}:{props.configuration.port})</h6> : null}
   </Link>
)