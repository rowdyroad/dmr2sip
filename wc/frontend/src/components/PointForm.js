import React, { Component, PropTypes } from 'react'
import * as UI from './UIKit'

const SIPForm = (props) => (
    <UI.Area>
        <UI.Row>
            <UI.Col8>
                <UI.Input label="SIP Host" name="host" value={props.host} onChange={props.onChange}/>
            </UI.Col8>
            <UI.Col4>
                <UI.Input label="SIP Port" name="port" value={props.port} onChange={props.onChange}/>
            </UI.Col4>
        </UI.Row>
        <UI.Row>
            <UI.Col6>
                <UI.Input label="Username" name="authorization_username"  value={props.authorization_username} onChange={props.onChange}/>
            </UI.Col6>
            <UI.Col6>
                <UI.Input type="password" name="password" label="Password" value={props.password} onChange={props.onChange}/>
            </UI.Col6>
        </UI.Row>
        <UI.Row>
            <UI.Col2>
                <UI.Input label="Caller ID" name="username" value={props.username} onChange={props.onChange}/>
            </UI.Col2>
        </UI.Row>
    </UI.Area>
)

const DMRForm = (props) => (
    <UI.Area>
        <UI.Row>
            <UI.Col4>
                <UI.Input label="Sound Device Index" name="device_index" value={props.device_index} onChange={props.onChange}/>
            </UI.Col4>
            <UI.Col4>
                <UI.Input label="Device Host" name="address" value={props.address} onChange={props.onChange}/>
            </UI.Col4>
            <UI.Col4>
                <UI.Input label="Device Port" name="port" value={props.port} onChange={props.onChange}/>
            </UI.Col4>
        </UI.Row>
        <h6 className="header-title">Phone mode interruption</h6>
        <UI.Row>
            <UI.Col6>
                <UI.Input label="Code" name="phone_mode.code" value={props.phone_mode ? props.phone_mode.code : ''} onChange={props.onChange}/>
            </UI.Col6>
            <UI.Col6>
                <UI.Input label="Duration(ms)" name="phone_mode.duration" value={props.phone_mode ? props.phone_mode.duration : ''} onChange={props.onChange}/>
            </UI.Col6>
        </UI.Row>
    </UI.Area>
)

const PointForm = (props) => (
    <UI.Form {...props}>
        <UI.Select label="Type" name="type" value={props.data.type} onChange={props.actions.change}>
            <option>-</option>
            <option value="dmr">DMR</option>
            <option value="sip">SIP</option>
        </UI.Select>
        <UI.Input label="Name" name="name"  value={props.data.name} onChange={props.actions.change}/>
        {props.data.type  === 'dmr'
            ? <DMRForm {...props.data.configuration} scope="configuration" onChange={props.actions.change}/> : null }

        {props.data.type === 'sip'
            ? <SIPForm {...props.data.configuration} scope="configuration" onChange={props.actions.change}/> : null }
        <UI.Button type="submit" disabled={props.saving || !props.dirty} onClick={props.onSubmit} icon ={props.saving ? "glyphicon glyphicon-refresh" : null} label="Save"/>
        &nbsp;
        { props.onClose ? <UI.Button type="button" color="white" disabled={props.saving} onClick={props.onClose} label="Close"/> : null}
    </UI.Form>
)

export default PointForm;