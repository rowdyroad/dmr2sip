import React, { Component, PropTypes } from 'react'
import * as UI from '../UIKit'

class Grid extends Component
{
    componentWillMount = () => {
        if (!this.props.data) {
            this.props.actions.fetch()
        }
    }

    render = () => {

        if (!this.props.data) {
            return null; //todo loader
        }

        return (
                    <div>
                        {this.props.newUrl
                            ? <UI.Button  to={this.props.newUrl}
                                    label="Add"
                                    icon="glyphicon glyphicon-plus-sign"
                                    style={{marginBottom:'1em',right:0}}/>
                            : null}
                        <UI.Grid cols={this.props.cols}
                                 items={this.props.data.data}
                                 component={this.props.component}
                                 />
                        {this.props.data._meta.currentPage < this.props.data._meta.pageCount ?
                            <UI.Button label="More" type="button" onClick={()=>{this.props.actions.fetch(this.props.data._meta.currentPage + 1,true)}}/>
                            : null }
                    </div>
                );
    }
}

Grid.propTypes = {
    cols: React.PropTypes.number.isRequired,
    component: React.PropTypes.any.isRequired,
    newUrl: React.PropTypes.string,
    data: React.PropTypes.object
}

export default Grid;