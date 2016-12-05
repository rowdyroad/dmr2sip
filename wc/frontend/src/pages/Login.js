import React, { Component } from 'react';
import { connect } from 'react-redux'
import PaperLayout from '../components/PaperLayout';
import * as Actions from '../actions'
import * as UI from '../components/UIKit'
import { reduxForm } from 'redux-form/immutable'
class Login extends Component
{
  render = () => {
    console.log(this.props);
    return (
            <PaperLayout>
             <form onSubmit={this.props.onSubmit}>
                {this.props.error == 422 ? <div style={{color:'red'}}>Incorrect login/password</div> : null}
                <UI.RFInput label="Username" name="username"/>
                <UI.RFInput label="Password" type="password" name="password"/>
                <UI.Button type="submit" disabled={this.props.pristine || this.props.submitting} label="Submit"/>
              </form>
            </PaperLayout>
          );
  }
}

export default Login;