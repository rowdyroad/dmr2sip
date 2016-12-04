import React from 'react'
import { FormSection } from 'redux-form/immutable'
import * as UI from '../../UIKit'

const SIPForm = (props) => (
    <UI.Area>
        <UI.Row>
            <UI.Col8>
                 <UI.RFInput label="SIP Host" name="host"/>
            </UI.Col8>
            <UI.Col4>
                <UI.RFInput label="SIP Port" name="port"/>
            </UI.Col4>
        </UI.Row>
        <UI.Row>
            <UI.Col6>
                 <UI.RFInput label="Username" name="authorization_username"/>
            </UI.Col6>
            <UI.Col6>
                 <UI.RFInput label="Password" name="password"/>
            </UI.Col6>
        </UI.Row>
        <UI.Row>
            <UI.Col2>
                 <UI.RFInput label="Caller ID" name="username"/>
            </UI.Col2>
        </UI.Row>
    </UI.Area>
)


const DMRFormPhoneMode = (props) => (
   <UI.Area>
      <h6 className="header-title">Phone mode interruption</h6>
        <UI.Row>
            <UI.Col6>
              <UI.RFInput label="Code" name="code"/>
            </UI.Col6>
            <UI.Col6>
              <UI.RFInput label="Duration(ms)" name="duration"/>
            </UI.Col6>
        </UI.Row>
  </UI.Area>
)

const DMRForm = (props) => (
    <UI.Area>
        <UI.Row>
            <UI.Col4>
                <UI.RFInput label="Device Index" name="device_index"/>
            </UI.Col4>
            <UI.Col4>
                <UI.RFInput label="Device Host" name="address"/>
            </UI.Col4>
            <UI.Col4>
                <UI.RFInput label="Device Port" name="port"/>
            </UI.Col4>
        </UI.Row>
        <FormSection name="phone_mode">
          <DMRFormPhoneMode/>
        </FormSection>
    </UI.Area>
)

const PointForm = (props) => (
    <UI.Box>
      <form onSubmit={props.onSubmit}>
          <UI.RFInput label="Name" name="name"/>
          <UI.RFSelect label="Type" name="type">
            <option></option>
            <option value="dmr">DMR</option>
            <option value="sip">SIP</option>
          </UI.RFSelect>
          {props.data ?
              <FormSection name="configuration">
              {props.data.type === 'dmr' ? <DMRForm/> : null}
              {props.data.type === 'sip' ? <SIPForm/> : null}
              </FormSection>
            : null
          }
          <UI.Button type="submit" disabled={props.pristine || props.submitting} label="Submit"/>
          &nbsp;
          <UI.Button type="button" color="white" onClick={props.onClose} label="Close"/>
      </form>
    </UI.Box>
)

export default PointForm;