import React, { Component, PropTypes } from 'react';
import { connect } from 'react-redux'
import logo from '../assets/images/logo-b.png';
import CenterLayout from './CenterLayout';

class PaperLayout extends Component
{
	render = () => {
		let isMobile = this.props.viewport.width < 480;
		return (<CenterLayout>
              <div className={isMobile ? "": "card-box"}>
                <div className="panel-body">
                  <h1 className="title">
                    <img src={logo} className="logo" alt="logo" />&nbsp;DMR2SIP</h1>
                    {this.props.children}
                </div>
            </div>
            </CenterLayout>
            );
	}
}

export default connect(state => { return state.main.toJS(); })(PaperLayout)