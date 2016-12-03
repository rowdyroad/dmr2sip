import React from 'react'
import * as UI from '../../UIKit'

const SIPView = (props) => (
    <div>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="SIP Host">{props.host}</UI.Property>
            </UI.Col2>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="SIP Port">{props.port}</UI.Property>
            </UI.Col2>
        </UI.Row>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Username">{props.authorization_username}</UI.Property>
            </UI.Col2>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Password">********</UI.Property>
            </UI.Col2>
        </UI.Row>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Caller ID">{props.username}</UI.Property>
            </UI.Col2>
        </UI.Row>
    </div>
)
const DMRView = (props) => (
    <UI.Area>
        <UI.Row>
            <UI.Col4 style={{padding:'1em'}}>
                <UI.Property label="Sound Device Index">{props.device_index}</UI.Property>
            </UI.Col4>
        </UI.Row>
        <UI.Row>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Device Host">{props.address}</UI.Property>
            </UI.Col2>
            <UI.Col2 style={{padding:'1em'}}>
                <UI.Property label="Device Port">{props.port}</UI.Property>
            </UI.Col2>
        </UI.Row>
        <h6 className="header-title">Phone mode interruption</h6>
        <UI.Row>
            <UI.Col4 style={{padding:'1em'}}>
                <UI.Property label="Code">{props.phone_mode.code}</UI.Property>
            </UI.Col4>
            <UI.Col4 style={{padding:'1em'}}>
                <UI.Property label="Duration">{props.phone_mode.duration} ms</UI.Property>
            </UI.Col4>
        </UI.Row>
    </UI.Area>
)

const PointView = (props) => {
    if (!props.data) {
        return null;
    }
    return (
            <UI.Box>
                <UI.Area>
                    <UI.Row>
                        <UI.Col10>
                            <h1>
                                <i className={"fa fa-circle point-status-" + props.data.status}></i>&nbsp;{props.data.name}
                                <small style={{ display:'block', fontSize:14, paddingLeft:40}}>{props.data.type}</small>
                            </h1>
                        </UI.Col10>
                        <UI.Col2 style={{textAlign:'right'}}>
                            <UI.Button to={'/points/' + props.data.point_id + '/edit'} icon="ti-pencil"/>
                            &nbsp;
                            {props.onRemove ? <UI.Button onClick={props.onRemove} color="danger" icon="ti-trash"/> : null}
                        </UI.Col2>
                    </UI.Row>
                    {props.data.type === 'dmr' ? <DMRView phone_mode={{}} {...props.data.configuration}/> : null}
                    {props.data.type === 'sip' ? <SIPView {...props.data.configuration || {}}/> : null}
                </UI.Area>
            </UI.Box>
        )
}

export default PointView;