import React from 'react'
import {Link} from 'react-router'
import * as UI from '../../UIKit'

const View = (props) => {
    if (!props.data) {
        return null;
    }
    return (
            <UI.Box>
                <UI.Area>
                    <UI.Row>
                        <UI.Col10>
                            <h1>
                                {props.data.name}
                                <small style={{ display:'block', fontSize:14}}>{props.data.username}</small>
                            </h1>
                        </UI.Col10>
                        <UI.Col2 style={{textAlign:'right'}}>
                            <UI.Button to={'/users/' + props.data.user_id + '/update'} icon="ti-pencil"/>
                        </UI.Col2>
                    </UI.Row>
                </UI.Area>
            </UI.Box>
        )
}

export default View;